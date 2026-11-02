/*
 * FishVisuals — мод для Bearite (Super Bear Adventure 13.0.8)
 */
#include "bearite_api.h"
#include "bearite_il2cpp.h"

#include <stdint.h>
#include <string.h>
#include <math.h>

typedef struct { float r, g, b, a; } Color;
typedef void (*set_color_fn)(void*, Color, const void*);
typedef void (*set_float_fn)(void*, float, const void*);
typedef void* (*get_obj_fn)(void*, const void*);
typedef void* (*go_type_fn)(void*, void*, const void*);

static const BeariteApi* g_api;
static const char* g_id;

static void* (*il_class_get_type)(void* klass);
static void* (*il_type_get_object)(void* type);

static void* mi_set_color;
static void* mi_get_gameobject;
static void* mi_go_getcomponent;
static void* mi_go_addcomponent;
static void* mi_trail_time;
static void* mi_trail_startwidth;
static void* mi_trail_endwidth;
static void* mi_trail_startcolor;
static void* mi_trail_endcolor;

static set_color_fn    fn_set_color;
static get_obj_fn      fn_get_gameobject;
static go_type_fn      fn_go_getcomponent;
static go_type_fn      fn_go_addcomponent;
static set_float_fn    fn_trail_time;
static set_float_fn    fn_trail_startwidth;
static set_float_fn    fn_trail_endwidth;
static set_color_fn    fn_trail_startcolor;
static set_color_fn    fn_trail_endcolor;

static void* g_trail_type;   
static void* g_trail;        
static long  g_tick;         
static float g_hue;

static Color hsv2rgb(float h) {
    float f = h * 6.f - (float)(int)(h * 6.f);
    Color c; c.a = 1.f;
    switch ((int)(h * 6.f) % 6) {
        case 0: c.r=1;     c.g=f;     c.b=0;     break;
        case 1: c.r=1.f-f; c.g=1;     c.b=0;     break;
        case 2: c.r=0;     c.g=1;     c.b=f;     break;
        case 3: c.r=0;     c.g=1.f-f; c.b=1;     break;
        case 4: c.r=f;     c.g=0;     c.b=1;     break;
        default:c.r=1;     c.g=0;     c.b=1.f-f; break;
    }
    return c;
}

static Color trail_color(const char* name) {
    if (!strcmp(name, "fire"))    return (Color){1.f, 0.35f, 0.05f, 1.f};
    if (!strcmp(name, "stars"))   return (Color){1.f, 0.9f,  0.2f,  1.f};
    if (!strcmp(name, "hearts"))  return (Color){1.f, 0.2f,  0.45f, 1.f};
    if (!strcmp(name, "rainbow")) return hsv2rgb(g_hue);
    return (Color){0.45f, 0.85f, 1.f, 1.f};  
}

static void ensure_trail(void* player) {
    if (!fn_get_gameobject || !g_trail_type) return;
    if (g_trail && (g_tick % 256) != 0) return;
    void* go = fn_get_gameobject(player, mi_get_gameobject);
    if (!go) return;
    g_trail = fn_go_getcomponent(go, g_trail_type, mi_go_getcomponent);
    if (!g_trail && fn_go_addcomponent)
        g_trail = fn_go_addcomponent(go, g_trail_type, mi_go_addcomponent);
    if (g_trail && (g_tick % 256) == 0)
        g_api->log(g_id, "трейл готов");
}

static void visuals_tick(void* player, float dt) {
    int    chroma_on  = g_api->get_bool(g_id, "enable_chroma", 0);
    int    trail_on   = g_api->get_bool(g_id, "enable_trail", 1);
    double chroma_spd = g_api->get_float(g_id, "chroma_speed", 2.0);
    void* cosmetics = *(void**)((char*)player + 0x150);   

    if (chroma_on && cosmetics && fn_set_color) {
        g_hue += 0.15f * (float)chroma_spd * dt;
        if (g_hue > 1.f) g_hue -= 1.f;
        Color c = hsv2rgb(g_hue);
        void* m1 = *(void**)((char*)cosmetics + 0x20);
        void* m2 = *(void**)((char*)cosmetics + 0x28);
        if (m1) fn_set_color(m1, c, mi_set_color);
        if (m2) fn_set_color(m2, c, mi_set_color);
    }

    if (trail_on) {
        ensure_trail(player);
        if (g_trail) {
            float t = (float)g_api->get_float(g_id, "trail_time", 1.0);
            float w = (float)g_api->get_float(g_id, "trail_width", 0.5);
            const char* tex = g_api->get_string(g_id, "trail_texture", "bubbles");
            Color c = trail_color(tex);
            Color e = c; e.a = 0.05f;
            fn_trail_time(g_trail, t, mi_trail_time);
            fn_trail_startwidth(g_trail, w, mi_trail_startwidth);
            fn_trail_endwidth(g_trail, 0.02f, mi_trail_endwidth);
            fn_trail_startcolor(g_trail, c, mi_trail_startcolor);
            fn_trail_endcolor(g_trail, e, mi_trail_endcolor);
        }
    }
}

static void (*orig_after_update)(void*, float, const void*);

static void hk_after_update(void* self, float dt, const void* mi) {
    orig_after_update(self, dt, mi);
    if (!self) return;
    g_tick++;
    if (*(uint8_t*)((char*)self + 0x138) == 0) return; 
    visuals_tick(self, dt);
}

static void (*orig_set_fov)(void*, float, const void*);

static void hk_set_fov(void* self, float value, const void* mi) {
    if (g_api->get_bool(g_id, "enable_fov", 0))
        value = (float)g_api->get_float(g_id, "custom_fov", 75.0);
    orig_set_fov(self, value, mi);
}

__attribute__((visibility("default")))
void bearite_mod_init(const BeariteApi* api, const char* mod_id) {
    g_api = api;
    g_id  = mod_id;
    if (api->version != BEARITE_API_VERSION) return;
    api->log(mod_id, "FishVisuals: загрузка (SBA 13.0.8)");

    if (!bearite_il2cpp_init(api)) {
        api->log(mod_id, "FishVisuals: il2cpp API не найден, мод не работает");
        return;
    }
    il_class_get_type  = (void*(*)(void*))api->il2cpp_symbol("il2cpp_class_get_type");
    il_type_get_object = (void*(*)(void*))api->il2cpp_symbol("il2cpp_type_get_object");

    mi_set_color        = bearite_find_method("UnityEngine", "Material",  "set_color", 1);
    mi_get_gameobject   = bearite_find_method("UnityEngine", "Component", "get_gameObject", 0);
    mi_go_getcomponent  = bearite_find_method("UnityEngine", "GameObject", "GetComponent", 1);
    mi_go_addcomponent  = bearite_find_method("UnityEngine", "GameObject", "AddComponent", 1);
    mi_trail_time       = bearite_find_method("UnityEngine", "TrailRenderer", "set_time", 1);
    mi_trail_startwidth = bearite_find_method("UnityEngine", "TrailRenderer", "set_startWidth", 1);
    mi_trail_endwidth   = bearite_find_method("UnityEngine", "TrailRenderer", "set_endWidth", 1);
    mi_trail_startcolor = bearite_find_method("UnityEngine", "TrailRenderer", "set_startColor", 1);
    mi_trail_endcolor   = bearite_find_method("UnityEngine", "TrailRenderer", "set_endColor", 1);

    fn_set_color        = (set_color_fn)bearite_method_code(mi_set_color);
    fn_get_gameobject   = (get_obj_fn)bearite_method_code(mi_get_gameobject);
    fn_go_getcomponent  = (go_type_fn)bearite_method_code(mi_go_getcomponent);
    fn_go_addcomponent  = (go_type_fn)bearite_method_code(mi_go_addcomponent);
    fn_trail_time       = (set_float_fn)bearite_method_code(mi_trail_time);
    fn_trail_startwidth = (set_float_fn)bearite_method_code(mi_trail_startwidth);
    fn_trail_endwidth   = (set_float_fn)bearite_method_code(mi_trail_endwidth);
    fn_trail_startcolor = (set_color_fn)bearite_method_code(mi_trail_startcolor);
    fn_trail_endcolor   = (set_color_fn)bearite_method_code(mi_trail_endcolor);

    void* trail_klass = bearite_find_class("UnityEngine", "TrailRenderer");
    if (trail_klass && il_class_get_type && il_type_get_object)
        g_trail_type = il_type_get_object(il_class_get_type(trail_klass));
    if (!g_trail_type) api->log(mod_id, "FishVisuals: System.Type TrailRenderer не получен, трейл недоступен");

    char msg[160];
    snprintf(msg, sizeof msg, "FishVisuals: chroma=%p trail=%p fov-hook...",
             (void*)fn_set_color, (void*)g_trail_type);
    api->log(mod_id, msg);

    bearite_hook_method(api, mod_id, "", "PlayerBearController", "AfterCharacterUpdate", 1,
                        (void*)hk_after_update, (void**)&orig_after_update);
    bearite_hook_method(api, mod_id, "UnityEngine", "Camera", "set_fieldOfView", 1,
                        (void*)hk_set_fov, (void**)&orig_set_fov);
}
