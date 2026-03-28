using System;
using System.Numerics;
using System.Threading;
using System.Threading.Tasks;
using System.Diagnostics;
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
            Console.Write("[?] Enter the exact game process name (without .exe, e.g., 'csgo'): ");
            string processName = Console.ReadLine() ?? "GameProcessName";

            Console.WriteLine("[*] Press 'Insert' to toggle the menu.");
            Console.WriteLine($"[*] Looking for process: {processName}");

            ESPOverlay overlay = new ESPOverlay(processName);

            Task.Run(() => overlay.Start().Wait());

            while (true)
            {
                Thread.Sleep(100);
            }
        }
    }

    public static class Offsets
    {
        // These base offsets are examples of how Unity IL2CPP handles GameObjectManager.
        // Since we only have the dump for classes and not the base signatures, these are placeholders.
        // You MUST find the actual ObjectManager / EntityList signature or pointer for your specific game.
        public static nint EntityList = 0x1234568;

        // SteamworksPlayerController offsets from dump.cs
        // private Vector3 ↈÁÂÃÃÃÀÂÃÀÃÁÁÁÀÀÃÂÂÂÂÀÁÂↈ; // 0x28 (Position likely)
        // private float ↈÁÃÃÁÂÂÃÃÃÀÃÂÂÀÁÂÂÂÀÃÀÁÁↈ; // 0x60 (Health likely)
        // private bool ↈÃÀÂÁÁÁÁÃÃÁÀÃÁÁÁÁÃÂÂÂÁÂÃↈ; // 0x64 (IsAlive/IsLocalPlayer likely)
        public static nint Position = 0x28;
        public static nint Health = 0x60;
        public static nint IsAlive = 0x64;

        // Unity typically stores ViewMatrix in Camera or global UnityPlayer.dll / GameAssembly.dll
        public static nint ViewMatrix = 0x567890;
    }

    class ESPOverlay : Overlay
    {
        private string processName;
        private bool showMenu = true;
        private bool enableESP = true;
        private bool showBox = true;
        private bool showSnaplines = true;
        private bool showHealth = true;
        private bool showDistance = true;
        private bool showName = true;

        private Vector4 boxColor = new Vector4(1f, 0f, 0f, 1f);
        private Vector4 snaplineColor = new Vector4(0f, 1f, 0f, 1f);
        private Vector4 nameColor = new Vector4(1f, 1f, 1f, 1f);

        private Swed? swed;
        private nint clientBase;
        private bool gameFound = false;

        public ESPOverlay(string procName)
        {
            processName = procName;
        }

        protected override void Render()
        {
            if (!gameFound)
            {
                // To avoid excessive CPU usage from exceptions in a tight render loop,
                // we first check if the process is running.
                Process[] processes = Process.GetProcessesByName(processName);
                if (processes.Length > 0)
                {
                    try
                    {
                        swed = new Swed(processName);
                        // For IL2CPP, it is usually GameAssembly.dll. For Mono, it's mono-2.0-bdwgc.dll.
                        clientBase = swed.GetModuleBase("GameAssembly.dll");
                        gameFound = true;
                        Console.WriteLine("[+] Game hooked successfully!");
                    }
                    catch
                    {
                        gameFound = false;
                    }
                }
            }
            else
            {
                // Check if the game was closed
                Process[] processes = Process.GetProcessesByName(processName);
                if (processes.Length == 0)
                {
                    gameFound = false;
                    swed = null;
                    Console.WriteLine("[-] Game closed. Waiting for it to start again...");
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
                ImGui.TextColored(new Vector4(1f, 0f, 0f, 1f), $"Game '{processName}.exe' Not Found. Waiting...");
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
                // Draw mock data so user can see it works while game is closed
                DrawMockData(drawList, screenBottom, screenSize);
                return;
            }

            try
            {
                // Note: The below logic is a standard structure. Because we lack the EXACT EntityList pointer
                // and ViewMatrix pointer (which are not in dump.cs but in memory), this will read 0s.
                // Replace Offsets.EntityList and Offsets.ViewMatrix with actual memory addresses for your game.

                // Read ViewMatrix (16 floats)
                float[] viewMatrix = ReadMatrix(clientBase + Offsets.ViewMatrix);

                nint entityList = swed.ReadPointer(clientBase + Offsets.EntityList);

                // Iterate through players (assuming 32 max for this example)
                for (int i = 0; i < 32; i++)
                {
                    nint entity = swed.ReadPointer(entityList + i * 0x8); // Adjust pointer arithmetic based on list structure
                    if (entity == 0) continue;

                    bool isAlive = swed.ReadBool(entity + Offsets.IsAlive);
                    if (!isAlive) continue;

                    Vector3 feetPos = swed.ReadVec(entity + Offsets.Position);
                    float health = swed.ReadFloat(entity + Offsets.Health);

                    // Assume player height is ~1.8 units
                    Vector3 headPos = feetPos + new Vector3(0, 1.8f, 0);

                    Vector2 screenFeet, screenHead;

                    if (WorldToScreen(feetPos, viewMatrix, screenSize, out screenFeet) &&
                        WorldToScreen(headPos, viewMatrix, screenSize, out screenHead))
                    {
                        float height = screenFeet.Y - screenHead.Y;
                        float width = height / 2f;

                        Vector2 boxMin = new Vector2(screenHead.X - width / 2, screenHead.Y);
                        Vector2 boxMax = new Vector2(screenFeet.X + width / 2, screenFeet.Y);

                        // Calculate mock distance since we don't have local player position easily
                        int distance = (int)Vector3.Distance(Vector3.Zero, feetPos);
                        int healthPct = Math.Clamp((int)health, 0, 100);

                        DrawPlayer(drawList, screenBottom, boxMin, boxMax, $"Enemy_{i}", distance, healthPct);
                    }
                }
            }
            catch (Exception)
            {
                gameFound = false;
            }
        }

        private float[] ReadMatrix(nint address)
        {
            if (swed == null) return new float[16];
            float[] matrix = new float[16];
            for (int i = 0; i < 16; i++)
            {
                matrix[i] = swed.ReadFloat(address + (i * 0x4));
            }
            return matrix;
        }

        private bool WorldToScreen(Vector3 pos, float[] matrix, Vector2 screenSize, out Vector2 screenPos)
        {
            screenPos = Vector2.Zero;

            float w = matrix[3] * pos.X + matrix[7] * pos.Y + matrix[11] * pos.Z + matrix[15];

            if (w < 0.01f) return false;

            float x = matrix[0] * pos.X + matrix[4] * pos.Y + matrix[8] * pos.Z + matrix[12];
            float y = matrix[1] * pos.X + matrix[5] * pos.Y + matrix[9] * pos.Z + matrix[13];

            x /= w;
            y /= w;

            screenPos.X = (screenSize.X / 2f) * (x + 1f);
            screenPos.Y = (screenSize.Y / 2f) * (1f - y);

            return true;
        }

        // Mock data to visualize the ESP if the game is not running or offsets are wrong
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
