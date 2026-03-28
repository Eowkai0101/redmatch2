#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include "il2cpp.h"
#include "minhook/include/MinHook.h"

// Define basic structs needed for Unity
struct Vector2 { float x, y; };
struct Vector3 {
    float x, y, z;
    static float Distance(Vector3 a, Vector3 b) {
        float dx = a.x - b.x;
        float dy = a.y - b.y;
        float dz = a.z - b.z;
        return sqrtf(dx*dx + dy*dy + dz*dz);
    }
};
struct Rect { float m_XMin, m_YMin, m_Width, m_Height; };
struct Color { float r, g, b, a; };

// Function pointers for IL2CPP
typedef void* (*il2cpp_domain_get_t)();
typedef void** (*il2cpp_domain_get_assemblies_t)(void* domain, size_t* size);
typedef void* (*il2cpp_assembly_get_image_t)(void* assembly);
typedef void* (*il2cpp_class_from_name_t)(void* image, const char* namespaze, const char* name);
typedef void* (*il2cpp_class_get_method_from_name_t)(void* klass, const char* name, int argsCount);
typedef void* (*il2cpp_class_get_field_from_name_t)(void* klass, const char* name);
typedef void (*il2cpp_field_static_get_value_t)(void* field, void* value);
typedef void (*il2cpp_field_get_value_t)(void* obj, void* field, void* value);
typedef void* (*il2cpp_runtime_invoke_t)(void* method, void* obj, void** params, void** exc);
typedef void* (*il2cpp_object_get_class_t)(void* obj);
typedef void* (*il2cpp_resolve_icall_t)(const char* name);
typedef void* (*il2cpp_string_new_t)(const char* str);
typedef void* (*il2cpp_type_get_object_t)(void* type);
typedef void* (*il2cpp_class_get_type_t)(void* klass);

il2cpp_domain_get_t il2cpp_domain_get = nullptr;
il2cpp_domain_get_assemblies_t il2cpp_domain_get_assemblies = nullptr;
il2cpp_assembly_get_image_t il2cpp_assembly_get_image = nullptr;
il2cpp_class_from_name_t il2cpp_class_from_name = nullptr;
il2cpp_class_get_method_from_name_t il2cpp_class_get_method_from_name = nullptr;
il2cpp_class_get_field_from_name_t il2cpp_class_get_field_from_name = nullptr;
il2cpp_field_static_get_value_t il2cpp_field_static_get_value = nullptr;
il2cpp_field_get_value_t il2cpp_field_get_value = nullptr;
il2cpp_runtime_invoke_t il2cpp_runtime_invoke = nullptr;
il2cpp_object_get_class_t il2cpp_object_get_class = nullptr;
il2cpp_resolve_icall_t il2cpp_resolve_icall = nullptr;
il2cpp_string_new_t il2cpp_string_new = nullptr;
il2cpp_type_get_object_t il2cpp_type_get_object = nullptr;
il2cpp_class_get_type_t il2cpp_class_get_type = nullptr;

struct Il2CppArray {
    void* klass;
    void* monitor;
    void* bounds;
    uintptr_t max_length;
    void* vector[1];
};

void* playerControllerClass = nullptr;
void* healthSyncerClass = nullptr;
void* cameraClass = nullptr;
void* componentClass = nullptr;
void* transformClass = nullptr;
void* guiClass = nullptr;
void* guiContentClass = nullptr;
void* objectClass = nullptr;
void* timeClass = nullptr;
void* behaviourClass = nullptr;

// Method caching to improve performance
void* get_mainCameraMethod = nullptr;
void* findObjMethod = nullptr;
void* pcTypeObj = nullptr;
void* localField = nullptr;
void* getTransformMethod = nullptr;
void* getComponentMethod = nullptr;
void* w2sMethod = nullptr;
void* guiLabelMethod = nullptr;
void* tempContentMethod = nullptr;
void* healthField = nullptr;
void* maxHealthField = nullptr;
void* hsTypeObj = nullptr;

bool InitIl2cpp() {
    HMODULE hAssembly = GetModuleHandleA("GameAssembly.dll");
    if (!hAssembly) return false;

    il2cpp_domain_get = (il2cpp_domain_get_t)GetProcAddress(hAssembly, "il2cpp_domain_get");
    il2cpp_domain_get_assemblies = (il2cpp_domain_get_assemblies_t)GetProcAddress(hAssembly, "il2cpp_domain_get_assemblies");
    il2cpp_assembly_get_image = (il2cpp_assembly_get_image_t)GetProcAddress(hAssembly, "il2cpp_assembly_get_image");
    il2cpp_class_from_name = (il2cpp_class_from_name_t)GetProcAddress(hAssembly, "il2cpp_class_from_name");
    il2cpp_class_get_method_from_name = (il2cpp_class_get_method_from_name_t)GetProcAddress(hAssembly, "il2cpp_class_get_method_from_name");
    il2cpp_class_get_field_from_name = (il2cpp_class_get_field_from_name_t)GetProcAddress(hAssembly, "il2cpp_class_get_field_from_name");
    il2cpp_field_static_get_value = (il2cpp_field_static_get_value_t)GetProcAddress(hAssembly, "il2cpp_field_static_get_value");
    il2cpp_field_get_value = (il2cpp_field_get_value_t)GetProcAddress(hAssembly, "il2cpp_field_get_value");
    il2cpp_runtime_invoke = (il2cpp_runtime_invoke_t)GetProcAddress(hAssembly, "il2cpp_runtime_invoke");
    il2cpp_object_get_class = (il2cpp_object_get_class_t)GetProcAddress(hAssembly, "il2cpp_object_get_class");
    il2cpp_resolve_icall = (il2cpp_resolve_icall_t)GetProcAddress(hAssembly, "il2cpp_resolve_icall");
    il2cpp_string_new = (il2cpp_string_new_t)GetProcAddress(hAssembly, "il2cpp_string_new");
    il2cpp_type_get_object = (il2cpp_type_get_object_t)GetProcAddress(hAssembly, "il2cpp_type_get_object");
    il2cpp_class_get_type = (il2cpp_class_get_type_t)GetProcAddress(hAssembly, "il2cpp_class_get_type");

    if (!il2cpp_domain_get) return false;

    size_t size = 0;
    void* domain = il2cpp_domain_get();
    void** assemblies = il2cpp_domain_get_assemblies(domain, &size);

    for (size_t i = 0; i < size; ++i) {
        void* image = il2cpp_assembly_get_image(assemblies[i]);
        if (!image) continue;

        if (!playerControllerClass) playerControllerClass = il2cpp_class_from_name(image, "", "PlayerController");
        if (!healthSyncerClass) healthSyncerClass = il2cpp_class_from_name(image, "", "HealthSyncer");
        if (!componentClass) componentClass = il2cpp_class_from_name(image, "UnityEngine", "Component");
        if (!transformClass) transformClass = il2cpp_class_from_name(image, "UnityEngine", "Transform");
        if (!cameraClass) cameraClass = il2cpp_class_from_name(image, "UnityEngine", "Camera");
        if (!guiClass) guiClass = il2cpp_class_from_name(image, "UnityEngine", "GUI");
        if (!objectClass) objectClass = il2cpp_class_from_name(image, "UnityEngine", "Object");
        if (!guiContentClass) guiContentClass = il2cpp_class_from_name(image, "UnityEngine", "GUIContent");
        if (!timeClass) timeClass = il2cpp_class_from_name(image, "UnityEngine", "Time");
        if (!behaviourClass) behaviourClass = il2cpp_class_from_name(image, "UnityEngine", "Behaviour");
    }

    if (!playerControllerClass || !healthSyncerClass || !cameraClass || !guiClass || !guiContentClass || !objectClass || !componentClass)
        return false;

    // Cache methods and fields
    get_mainCameraMethod = il2cpp_class_get_method_from_name(cameraClass, "get_main", 0);
    findObjMethod = il2cpp_class_get_method_from_name(objectClass, "FindObjectsOfType", 1);
    pcTypeObj = il2cpp_type_get_object(il2cpp_class_get_type(playerControllerClass));
    localField = il2cpp_class_get_field_from_name(playerControllerClass, "LocalInstance");
    getTransformMethod = il2cpp_class_get_method_from_name(componentClass, "get_transform", 0);
    getComponentMethod = il2cpp_class_get_method_from_name(componentClass, "GetComponent", 1);
    w2sMethod = il2cpp_class_get_method_from_name(cameraClass, "WorldToScreenPoint", 1);
    guiLabelMethod = il2cpp_class_get_method_from_name(guiClass, "Label", 2);
    tempContentMethod = il2cpp_class_get_method_from_name(guiContentClass, "Temp", 1);
    healthField = il2cpp_class_get_field_from_name(healthSyncerClass, "health");
    maxHealthField = il2cpp_class_get_field_from_name(healthSyncerClass, "maxHealth");
    hsTypeObj = il2cpp_type_get_object(il2cpp_class_get_type(healthSyncerClass));

    return true;
}

Vector3 GetPosition(void* transform) {
    Vector3 pos = {0,0,0};
    if (!transform || !transformClass) return pos;
    void* getPosMethod = il2cpp_class_get_method_from_name(transformClass, "get_position", 0);
    if(getPosMethod) {
        void* res = il2cpp_runtime_invoke(getPosMethod, transform, nullptr, nullptr);
        if(res) {
             pos = *(Vector3*)((uintptr_t)res + 0x10);
        }
    }
    return pos;
}

void DrawString(Rect rect, const char* text) {
    if (!guiLabelMethod || !tempContentMethod) return;

    void* strObj = il2cpp_string_new(text);
    void* args1[2] = { strObj };
    void* content = il2cpp_runtime_invoke(tempContentMethod, nullptr, args1, nullptr);

    void* args2[2] = { &rect, content };
    il2cpp_runtime_invoke(guiLabelMethod, nullptr, args2, nullptr);
}

// Function pointer and original pointer for our hook
typedef void (*Camera_OnGUI_t)(void* instance);
Camera_OnGUI_t o_CameraOnGUI = nullptr;

void hk_CameraOnGUI(void* instance) {
    // Call the original method first
    if (o_CameraOnGUI) o_CameraOnGUI(instance);

    if (!findObjMethod || !pcTypeObj) return;

    void* mainCamera = nullptr;
    if (get_mainCameraMethod) mainCamera = il2cpp_runtime_invoke(get_mainCameraMethod, nullptr, nullptr, nullptr);
    if (!mainCamera) return;

    void* args[1] = { pcTypeObj };
    Il2CppArray* players = (Il2CppArray*)il2cpp_runtime_invoke(findObjMethod, nullptr, args, nullptr);
    if (!players) return;

    void* localPlayer = nullptr;
    if (localField) il2cpp_field_static_get_value(localField, &localPlayer);

    Vector3 localPos = {0,0,0};
    if (localPlayer) {
        void* lt = il2cpp_runtime_invoke(getTransformMethod, localPlayer, nullptr, nullptr);
        if (lt) localPos = GetPosition(lt);
    }

    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    for (uintptr_t i = 0; i < players->max_length; i++) {
        void* player = players->vector[i];
        if (!player || player == localPlayer) continue;

        void* transform = il2cpp_runtime_invoke(getTransformMethod, player, nullptr, nullptr);
        if (!transform) continue;

        Vector3 pos = GetPosition(transform);
        float distance = Vector3::Distance(localPos, pos);

        int hp = 100, maxHp = 100;
        void* compArgs[1] = { hsTypeObj };
        void* hsObj = il2cpp_runtime_invoke(getComponentMethod, player, compArgs, nullptr);
        if (hsObj && healthField && maxHealthField) {
            il2cpp_field_get_value(hsObj, healthField, &hp);
            il2cpp_field_get_value(hsObj, maxHealthField, &maxHp);
        }

        if (w2sMethod) {
            void* w2sArgs[1] = { &pos };
            void* screenPosObj = il2cpp_runtime_invoke(w2sMethod, mainCamera, w2sArgs, nullptr);
            if (screenPosObj) {
                Vector3 screenPos = *(Vector3*)((uintptr_t)screenPosObj + 0x10);

                if (screenPos.z > 0.0f) {
                    char buffer[256];
                    sprintf_s(buffer, sizeof(buffer), "Enemy\nHP: %d/%d\nDist: %.1fm", hp, maxHp, distance);

                    Rect r = { screenPos.x - 50.0f, (float)screenHeight - screenPos.y, 200.0f, 100.0f };
                    DrawString(r, buffer);
                }
            }
        }
    }
}

DWORD WINAPI MainThread(LPVOID lpParam) {
    // Wait for the game to fully load before finding methods
    Sleep(5000);

    if (InitIl2cpp()) {
        MH_Initialize();

        // We hook a standard MonoBehaviour method on PlayerController or any component we know fires.
        // Let's hook the PlayerController Update or a similar method. But we want OnGUI.
        // The safest and easiest generic OnGUI in Unity is on an existing script. Let's look up PlayerController.
        // wait, does PlayerController have OnGUI? Let's check or hook Camera::OnGUI, if it exists? Camera is a component but doesn't have OnGUI natively in il2cpp.
        // Instead, we will just hook the PlayerController Update and create our own GUI GameObject or use an existing GUI script.
        // Actually, `PlayerController` has MonoBehaviour as base. So it can have OnGUI.
        // For generic ESP hooks, usually people hook `UnityEngine.GUI::CallWindowDelegate` or `UnityEngine.Event::PopEvent` if they want to render in OnGUI.
        // Let's hook the GUI.Label method itself, and inside of it, if it's drawing, we do our drawing once.
        // Or even better, we just hook `UnityEngine.GUI::DoLabel`. Let's hook `UnityEngine.Camera::OnPreRender` or `UnityEngine.Camera::FireOnPreRender`.

        // As a simpler approach for standard internal Unity hacks, we can find a native MonoBehaviour like `PlayerManager` or `PlayerController` that is likely updated.
        // Wait, the user specifically asked for "OnGUI metodu kullan" (use OnGUI method).
        // Let's find any OnGUI in the game assembly, or hook a Unity internal function that dispatches OnGUI.
        // A common hook target for GUI is "UnityEngine.GUIUtility::BeginGUI"
        void* guiUtilityClass = nullptr;
        size_t size = 0;
        void** assemblies = il2cpp_domain_get_assemblies(il2cpp_domain_get(), &size);
        for (size_t i = 0; i < size; ++i) {
            void* image = il2cpp_assembly_get_image(assemblies[i]);
            if (!guiUtilityClass) guiUtilityClass = il2cpp_class_from_name(image, "UnityEngine", "GUIUtility");
        }

        void* beginGuiMethod = nullptr;
        if (guiUtilityClass) {
            beginGuiMethod = il2cpp_class_get_method_from_name(guiUtilityClass, "BeginGUI", 1);
        }

        if (beginGuiMethod) {
            // IL2CPP MethodInfo contains methodPointer.
            void* methodPointer = *(void**)beginGuiMethod; // MethodInfo->methodPointer is the first element
            MH_CreateHook(methodPointer, (LPVOID)hk_CameraOnGUI, (LPVOID*)&o_CameraOnGUI);
            MH_EnableHook(methodPointer);
        } else {
            // Fallback: Hook PlayerController's Update method if OnGUI isn't accessible cleanly
            void* pcUpdateMethod = il2cpp_class_get_method_from_name(playerControllerClass, "Update", 0);
            if (pcUpdateMethod) {
                void* methodPointer = *(void**)pcUpdateMethod;
                MH_CreateHook(methodPointer, (LPVOID)hk_CameraOnGUI, (LPVOID*)&o_CameraOnGUI);
                MH_EnableHook(methodPointer);
            }
        }
    }
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(0, 0, MainThread, hModule, 0, 0);
    }
    return TRUE;
}
