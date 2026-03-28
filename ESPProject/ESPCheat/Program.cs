using System;
using System.Numerics;
using System.Threading;
using System.Threading.Tasks;
using ClickableTransparentOverlay;
using ImGuiNET;
using Swed64;

namespace ESPCheat
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("[*] Starting External ESP...");
            Console.WriteLine("[*] Press 'Insert' to toggle the menu.");
            Console.WriteLine("[*] Make sure the game is running.");

            ESPOverlay overlay = new ESPOverlay();

            Task.Run(() => overlay.Start().Wait());

            while (true)
            {
                Thread.Sleep(100);
            }
        }
    }

    // Offset definitions based on dump.cs inspection
    // Note: Dissonance.Integrations.SteamworksP2P.Demo.SteamworksPlayerController
    public static class Offsets
    {
        // Example offsets mapped from typical IL2CPP GameObjects / Components
        // These are placeholders since actual memory hunting is required in the real game.
        // In a real scenario, you iterate the ObjectManager or use a known signature/base pointer.
        public static nint PlayerList = 0x1234568; // Dummy address for the entity list
        public static nint PlayerCount = 0x10;

        // SteamworksPlayerController fields derived from dump.cs
        // private IDissonancePlayer ↈÀÃÁÀÃÂÃÂÂÂÃÀÀÃÃÃÂÂÃÃÃÃÁↈ; // 0x18
        // private CharacterController ↈÂÃÁÃÁÃÀÁÃÁÂÁÃÀÁÀÁÁÁÂÃÃÁↈ; // 0x20
        // private Vector3 ↈÁÂÃÃÃÀÂÃÀÃÁÁÁÀÀÃÂÂÂÂÀÁÂↈ; // 0x28
        // private Quaternion ↈÀÁÃÃÀÂÂÁÃÀÀÂÃÁÃÀÁÂÁÃÃÀÀↈ; // 0x34
        // private Vector3 ↈÀÂÂÃÁÁÁÃÃÃÃÀÀÂÀÃÁÁÁÃÂÀÁↈ; // 0x44
        // private Quaternion ↈÃÂÂÃÃÂÀÃÃÃÂÃÂÁÂÃÃÂÂÀÂÀÀↈ; // 0x50
        // private float ↈÁÃÃÁÂÂÃÃÃÀÃÂÂÀÁÂÂÂÀÃÀÁÁↈ; // 0x60
        // private bool ↈÃÀÂÁÁÁÁÃÃÁÀÃÁÁÁÁÃÂÂÂÁÂÃↈ; // 0x64

        public static nint DissonancePlayer = 0x18;
        public static nint CharacterController = 0x20;
        public static nint Vector3Pos1 = 0x28;
        public static nint Vector3Pos2 = 0x44;
        public static nint FloatVal = 0x60;
        public static nint BoolVal = 0x64;
    }

    class ESPOverlay : Overlay
    {
        private bool showMenu = true;
        private bool enableESP = true;
        private bool showBox = true;
        private bool showSnaplines = true;
        private bool showHealth = true;
        private bool showDistance = true;
        private bool showName = true;

        private Vector4 boxColor = new Vector4(1f, 0f, 0f, 1f); // Red
        private Vector4 snaplineColor = new Vector4(0f, 1f, 0f, 1f); // Green
        private Vector4 nameColor = new Vector4(1f, 1f, 1f, 1f); // White

        private Swed? swed;
        private nint clientBase;
        private bool gameFound = false;

        protected override void Render()
        {
            if (!gameFound)
            {
                try
                {
                    // Hooking to the game process using Swed64
                    swed = new Swed("GameProcessName");
                    clientBase = swed.GetModuleBase("GameAssembly.dll");
                    gameFound = true;
                }
                catch
                {
                    gameFound = false;
                }
            }

            if (ImGui.IsKeyPressed(ImGuiKey.Insert))
            {
                showMenu = !showMenu;
            }

            if (showMenu)
            {
                DrawMenu();
            }

            if (enableESP)
            {
                DrawESP();
            }
        }

        private void DrawMenu()
        {
            ImGui.Begin("C# External ESP Menu", ref showMenu, ImGuiWindowFlags.AlwaysAutoResize);

            if (!gameFound)
            {
                ImGui.TextColored(new Vector4(1f, 0f, 0f, 1f), "Game Not Found. Waiting for process...");
            }
            else
            {
                ImGui.TextColored(new Vector4(0f, 1f, 0f, 1f), "Game Hooked Successfully!");
            }

            ImGui.Separator();

            ImGui.Checkbox("Enable ESP", ref enableESP);
            ImGui.Checkbox("Draw Boxes", ref showBox);
            ImGui.Checkbox("Draw Snaplines", ref showSnaplines);
            ImGui.Checkbox("Draw Distance", ref showDistance);
            ImGui.Checkbox("Draw Health Bar", ref showHealth);
            ImGui.Checkbox("Draw Player Names", ref showName);

            ImGui.Separator();
            ImGui.ColorEdit4("Box Color", ref boxColor);
            ImGui.ColorEdit4("Snapline Color", ref snaplineColor);
            ImGui.ColorEdit4("Text Color", ref nameColor);

            ImGui.Separator();
            ImGui.Text("Press [Insert] to toggle this menu.");
            ImGui.End();
        }

        private void DrawESP()
        {
            var drawList = ImGui.GetBackgroundDrawList();
            Vector2 screenSize = ImGui.GetIO().DisplaySize;
            Vector2 screenBottom = new Vector2(screenSize.X / 2, screenSize.Y);

            if (!gameFound || swed == null)
            {
                DrawMockData(drawList, screenBottom, screenSize);
                return;
            }

            try
            {
                // This is the memory reading implementation using the actual offsets
                // found in dump.cs for the `SteamworksPlayerController` class.

                nint entityList = swed.ReadPointer(clientBase + Offsets.PlayerList);
                int playerCount = swed.ReadInt(clientBase + Offsets.PlayerCount);

                for (int i = 0; i < playerCount; i++)
                {
                    nint entity = swed.ReadPointer(entityList + i * 0x8);
                    if (entity == 0) continue;

                    // Read the positions from the SteamworksPlayerController structure
                    Vector3 worldPos = swed.ReadVec(entity + Offsets.Vector3Pos1);
                    float healthFloat = swed.ReadFloat(entity + Offsets.FloatVal);
                    bool isAlive = swed.ReadBool(entity + Offsets.BoolVal);

                    if (!isAlive) continue;

                    // World to screen conversion would happen here
                    // Vector2 screenPos = WorldToScreen(worldPos, viewMatrix);
                    // Vector2 headPos = WorldToScreen(worldPos + new Vector3(0, 1.8f, 0), viewMatrix);

                    // Since we don't have the view matrix in the dump, we skip actual rendering
                    // logic inside this block, but this demonstrates reading real offsets.
                }
            }
            catch (Exception)
            {
                // Process likely closed or read failed
                gameFound = false;
            }
        }

        // Mock data to visualize the ESP if the game is not running
        private void DrawMockData(ImDrawListPtr drawList, Vector2 screenBottom, Vector2 screenSize)
        {
            float baseX = screenSize.X / 2;
            float baseY = screenSize.Y / 2;

            DrawPlayer(drawList, screenBottom, new Vector2(baseX - 50, baseY - 100), new Vector2(baseX + 50, baseY + 100), "Enemy_1", 15, 100);
            DrawPlayer(drawList, screenBottom, new Vector2(baseX + 200, baseY - 50), new Vector2(baseX + 270, baseY + 100), "Enemy_2", 27, 50);
        }

        private void DrawPlayer(ImDrawListPtr drawList, Vector2 screenBottom, Vector2 boxMin, Vector2 boxMax, string name, int distance, int healthPercentage)
        {
            if (showBox)
            {
                drawList.AddRect(boxMin, boxMax, ImGui.ColorConvertFloat4ToU32(boxColor), 0.0f, ImDrawFlags.None, 1.5f);
            }
            if (showSnaplines)
            {
                drawList.AddLine(screenBottom, new Vector2(boxMin.X + (boxMax.X - boxMin.X) / 2, boxMax.Y), ImGui.ColorConvertFloat4ToU32(snaplineColor), 1.5f);
            }
            if (showName)
            {
                drawList.AddText(new Vector2(boxMin.X + 25, boxMin.Y - 20), ImGui.ColorConvertFloat4ToU32(nameColor), name);
            }
            if (showDistance)
            {
                drawList.AddText(new Vector2(boxMin.X + 35, boxMax.Y + 5), ImGui.ColorConvertFloat4ToU32(nameColor), $"[ {distance}m ]");
            }
            if (showHealth)
            {
                float height = boxMax.Y - boxMin.Y;
                float healthHeight = height * (healthPercentage / 100f);

                drawList.AddRectFilled(new Vector2(boxMin.X - 10, boxMin.Y), new Vector2(boxMin.X - 5, boxMax.Y), ImGui.ColorConvertFloat4ToU32(new Vector4(0f, 0f, 0f, 1f)));
                drawList.AddRectFilled(new Vector2(boxMin.X - 9, boxMin.Y + (height - healthHeight) + 1), new Vector2(boxMin.X - 6, boxMax.Y - 1), ImGui.ColorConvertFloat4ToU32(new Vector4(0f, 1f, 0f, 1f)));
            }
        }
    }
}
