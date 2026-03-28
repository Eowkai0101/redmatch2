#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include "il2cpp.h"
#include "minhook/include/MinHook.h"

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
typedef void* (*il2cpp_class_get_methods_t)(void* klass, void** iter);
typedef const char* (*il2cpp_method_get_name_t)(void* method);
typedef uint32_t (*il2cpp_method_get_param_count_t)(void* method);
typedef void* (*il2cpp_method_get_param_t)(void* method, uint32_t index);
typedef void* (*il2cpp_class_from_type_t)(void* type);

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
il2cpp_class_get_methods_t il2cpp_class_get_methods = nullptr;
il2cpp_method_get_name_t il2cpp_method_get_name = nullptr;
il2cpp_method_get_param_count_t il2cpp_method_get_param_count = nullptr;
il2cpp_method_get_param_t il2cpp_method_get_param = nullptr;
il2cpp_class_from_type_t il2cpp_class_from_type = nullptr;

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
void* screenClass = nullptr;
void* typeClass = nullptr;
void* eventClass = nullptr;

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
void* get_heightMethod = nullptr;
void* get_typeMethod = nullptr;

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
    il2cpp_class_get_methods = (il2cpp_class_get_methods_t)GetProcAddress(hAssembly, "il2cpp_class_get_methods");
    il2cpp_method_get_name = (il2cpp_method_get_name_t)GetProcAddress(hAssembly, "il2cpp_method_get_name");
    il2cpp_method_get_param_count = (il2cpp_method_get_param_count_t)GetProcAddress(hAssembly, "il2cpp_method_get_param_count");
    il2cpp_method_get_param = (il2cpp_method_get_param_t)GetProcAddress(hAssembly, "il2cpp_method_get_param");
    il2cpp_class_from_type = (il2cpp_class_from_type_t)GetProcAddress(hAssembly, "il2cpp_class_from_type");

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
        if (!screenClass) screenClass = il2cpp_class_from_name(image, "UnityEngine", "Screen");
        if (!timeClass) timeClass = il2cpp_class_from_name(image, "UnityEngine", "Time");
        if (!behaviourClass) behaviourClass = il2cpp_class_from_name(image, "UnityEngine", "Behaviour");
        if (!typeClass) typeClass = il2cpp_class_from_name(image, "System", "Type");
        if (!eventClass) eventClass = il2cpp_class_from_name(image, "UnityEngine", "Event");
    }

    if (!playerControllerClass || !healthSyncerClass || !cameraClass || !guiClass || !guiContentClass || !objectClass || !componentClass || !typeClass || !eventClass)
        return false;

    get_mainCameraMethod = il2cpp_class_get_method_from_name(cameraClass, "get_main", 0);
    findObjMethod = il2cpp_class_get_method_from_name(objectClass, "FindObjectsOfType", 1);
    pcTypeObj = il2cpp_type_get_object(il2cpp_class_get_type(playerControllerClass));
    localField = il2cpp_class_get_field_from_name(playerControllerClass, "LocalInstance");
    getTransformMethod = il2cpp_class_get_method_from_name(componentClass, "get_transform", 0);

    void* iter = nullptr;
    while (void* method = il2cpp_class_get_methods(componentClass, &iter)) {
        if (strcmp(il2cpp_method_get_name(method), "GetComponent") == 0 && il2cpp_method_get_param_count(method) == 1) {
            void* paramType = il2cpp_method_get_param(method, 0);
            if (il2cpp_class_from_type(paramType) == typeClass) {
                getComponentMethod = method;
                break;
            }
        }
    }

    w2sMethod = il2cpp_class_get_method_from_name(cameraClass, "WorldToScreenPoint", 1);

    iter = nullptr;
    while (void* method = il2cpp_class_get_methods(guiClass, &iter)) {
        if (strcmp(il2cpp_method_get_name(method), "Label") == 0 && il2cpp_method_get_param_count(method) == 2) {
            void* paramType = il2cpp_method_get_param(method, 1);
            if (il2cpp_class_from_type(paramType) == guiContentClass) {
                guiLabelMethod = method;
                break;
            }
        }
    }

    tempContentMethod = il2cpp_class_get_method_from_name(guiContentClass, "Temp", 1);
    healthField = il2cpp_class_get_field_from_name(healthSyncerClass, "health");
    maxHealthField = il2cpp_class_get_field_from_name(healthSyncerClass, "maxHealth");
    hsTypeObj = il2cpp_type_get_object(il2cpp_class_get_type(healthSyncerClass));
    get_heightMethod = il2cpp_class_get_method_from_name(screenClass, "get_height", 0);
    get_typeMethod = il2cpp_class_get_method_from_name(eventClass, "get_type", 0);

    return true;
}

Vector3 GetPosition(void* transform) {
    Vector3 pos = {0,0,0};
    if (!transform || !transformClass) return pos;
    void* getPosMethod = il2cpp_class_get_method_from_name(transformClass, "get_position", 0);
    if(getPosMethod) {
        void* res = il2cpp_runtime_invoke(getPosMethod, transform, nullptr, nullptr);
        if(res) pos = *(Vector3*)((uintptr_t)res + 0x10);
    }
    return pos;
}

void DrawString(Rect rect, const char* text) {
    if (!guiLabelMethod || !tempContentMethod) return;
    void* strObj = il2cpp_string_new(text);
    void* args1[1] = { strObj };
    void* content = il2cpp_runtime_invoke(tempContentMethod, nullptr, args1, nullptr);
    void* args2[2] = { &rect, content };
    il2cpp_runtime_invoke(guiLabelMethod, nullptr, args2, nullptr);
}

// Caching Players to avoid lag
std::vector<void*> cachedPlayers;
DWORD lastCacheTime = 0;

void UpdatePlayerCache() {
    DWORD currentTime = GetTickCount();
    if (currentTime - lastCacheTime < 1000) return; // Cache once per second
    lastCacheTime = currentTime;

    if (!findObjMethod || !pcTypeObj) return;
    void* args[1] = { pcTypeObj };
    Il2CppArray* players = (Il2CppArray*)il2cpp_runtime_invoke(findObjMethod, nullptr, args, nullptr);
    if (!players) return;

    cachedPlayers.clear();
    for (uintptr_t i = 0; i < players->max_length; i++) {
        if (players->vector[i]) {
            cachedPlayers.push_back(players->vector[i]);
        }
    }
}

// Hook GUIUtility.BeginGUI
typedef void (*BeginGUI_t)(int, int, int);
BeginGUI_t o_BeginGUI = nullptr;

void hk_BeginGUI(int a1, int a2, int a3) {
    if (o_BeginGUI) o_BeginGUI(a1, a2, a3);

    // Only draw during Repaint event type (EventType.Repaint == 7)
    if (get_typeMethod) {
        void* currentEventMethod = il2cpp_class_get_method_from_name(eventClass, "get_current", 0);
        if (currentEventMethod) {
            void* currentEv = il2cpp_runtime_invoke(currentEventMethod, nullptr, nullptr, nullptr);
            if (currentEv) {
                void* typeObj = il2cpp_runtime_invoke(get_typeMethod, currentEv, nullptr, nullptr);
                if (typeObj) {
                    int eventType = *(int*)((uintptr_t)typeObj + 0x10);
                    if (eventType != 7) return; // Not repaint
                }
            }
        }
    }

    UpdatePlayerCache();

    void* mainCamera = nullptr;
    if (get_mainCameraMethod) mainCamera = il2cpp_runtime_invoke(get_mainCameraMethod, nullptr, nullptr, nullptr);
    if (!mainCamera) return;

    void* localPlayer = nullptr;
    if (localField) il2cpp_field_static_get_value(localField, &localPlayer);

    Vector3 localPos = {0,0,0};
    if (localPlayer) {
        void* lt = il2cpp_runtime_invoke(getTransformMethod, localPlayer, nullptr, nullptr);
        if (lt) localPos = GetPosition(lt);
    }

    int screenHeight = 1080;
    if (get_heightMethod) {
        void* hObj = il2cpp_runtime_invoke(get_heightMethod, nullptr, nullptr, nullptr);
        if (hObj) screenHeight = *(int*)((uintptr_t)hObj + 0x10);
    }

    for (void* player : cachedPlayers) {
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
    Sleep(5000);

    if (InitIl2cpp()) {
        MH_Initialize();

        void* guiUtilityClass = nullptr;
        size_t size = 0;
        void** assemblies = il2cpp_domain_get_assemblies(il2cpp_domain_get(), &size);
        for (size_t i = 0; i < size; ++i) {
            void* image = il2cpp_assembly_get_image(assemblies[i]);
            if (!guiUtilityClass) guiUtilityClass = il2cpp_class_from_name(image, "UnityEngine", "GUIUtility");
        }

        void* beginGuiMethod = nullptr;
        if (guiUtilityClass) {
            beginGuiMethod = il2cpp_class_get_method_from_name(guiUtilityClass, "BeginGUI", 3);
        }

        if (beginGuiMethod) {
            void* methodPointer = *(void**)beginGuiMethod;
            MH_CreateHook(methodPointer, (LPVOID)hk_BeginGUI, (LPVOID*)&o_BeginGUI);
            MH_EnableHook(methodPointer);
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
