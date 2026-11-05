/*
 * FishVisuals v1.16 - фикс перегрузки HasProperty: безусловная запись цветов
 * Хрома-скин (радуга), трейл, FOV/aspect, пакет безумия, диско-небо, slo-mo.
 * v1.3: пресеты неба + интенсивность, динамический FOV по скорости,
 *       годзилла-медведь (масштаб), стробоскоп хромы, пульс времени.
 *
 * Дамп: dump.cs 13.0.8 (Assembly-CSharp). Оффсеты полей (проверены по дампу):
 *   PlayerBearController.isLocalPlayer        0x138
 *   PlayerBearController.cosmeticsController  0x150
 *   BearCosmetics.skinMaterial                0x20
 *   BearCosmetics.shickaMaterial              0x28
 *   BearCosmetics.headTransform               0x38
 *   BearCosmetics.bodyTransform               0x40
 *   BearCosmetics.shicka                      0x48
 *   BearCosmetics.characterRenderersController 0x58
 *   CharacterRenderersController.characterRootTransform 0x20
 * Методы Unity/SBA резолвятся по именам через bearite_il2cpp (без адресов).
 * Пер-frame точка: PlayerBearController.AfterCharacterUpdate(float).
 */
#include "bearite_api.h"
#include "bearite_il2cpp.h"

#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

typedef struct { float r, g, b, a; } Color;
typedef struct { float x, y, z; } Vec3;
typedef struct { float x, y, z, w; } Quat;
typedef void (*tr_vec3_fn)(void*, Vec3, const void*);
typedef void (*set_ambient_fn)(Color, const void*);
typedef Vec3 (*get_euler_fn)(void*, const void*);
typedef void (*set_int_fn)(void*, int, const void*);
typedef void (*set_aspect_fn)(void*, float, const void*);
typedef void* (*static_obj_fn)(const void*);
typedef Vec3 (*get_vec3_fn)(void*, const void*);
typedef Vec3 (*get_pos_fn)(void*, const void*);   /* Transform.get_position (world) */
typedef void (*set_color_fn)(void*, Color, const void*);
typedef void* (*shader_find_fn)(const void*, const void*);
typedef void* (*instantiate_fn)(void*, const void*);
typedef void  (*set_shader_fn)(void*, void*, const void*);
typedef void  (*set_material_fn)(void*, void*, const void*);
typedef void (*set_float_fn)(void*, float, const void*);
typedef void* (*get_obj_fn)(void*, const void*);
typedef void* (*go_type_fn)(void*, void*, const void*);
typedef void (*static_float_fn)(float, const void*);   /* static Time.set_timeScale(float) */
typedef void (*static_bool_fn)(int, const void*);      /* static RenderSettings.set_fog(bool) */
typedef void (*static_int_fn)(int, const void*);       /* static RenderSettings.set_ambientMode(enum) */
typedef void  (*mat_setcolor2_fn)(void*, void*, Color, const void*); /* Material.SetColor(string, Color) */
typedef Color (*mat_getcolor_fn)(void*, void*, const void*);         /* Material.GetColor(string) */
typedef int   (*mat_hasprop_fn)(void*, void*, const void*);          /* Material.HasProperty(string) */
typedef void  (*set_pos_fn)(void*, Vec3, const void*);               /* Transform.set_position */
typedef void  (*set_rot_fn)(void*, Quat, const void*);                /* Transform.set_rotation */
typedef void  (*set_enabled_fn)(void*, int, const void*);              /* Behaviour.set_enabled */
typedef void* (*go_ctor_fn)(void*, void*, const void*);                 /* GameObject..ctor(string) */
typedef void  (*go_setactive_fn)(void*, int, const void*);               /* GameObject.SetActive(bool) */
typedef void  (*set_parent_fn)(void*, void*, const void*);               /* Transform.set_parent(Transform) */
typedef void  (*cam_replshader_fn)(void*, void*, void*, const void*);    /* Camera.SetReplacementShader(Shader, string) */
typedef void  (*cam_resetshader_fn)(void*, const void*);                 /* Camera.ResetReplacementShader() */
typedef void  (*set_bgcolor_fn)(void*, Color, const void*);               /* Camera.set_backgroundColor */
typedef void  (*set_clearflags_fn)(void*, int, const void*);             /* Camera.set_clearFlags(int) */
typedef void* (*find_objects_fn)(void*, const void*);
typedef void* (*alloc_obj_fn)(void*);                    /* il2cpp_object_new(Il2CppClass*) */
typedef struct { uint8_t r, g, b, a; } Color32;
typedef void (*tmp_set_text_fn)(void*, void*, const void*);   /* TMPro.TMP_Text.set_text(string) */
typedef Quat (*get_rot_fn)(void*, const void*);               /* Transform.get_rotation */
typedef void (*tmp_set_color_fn)(void*, Color, const void*);  /* TMPro.TMP_Text.set_color(Color) */
typedef void* (*ps_get_emission_fn)(void*, const void*);     /* ParticleSystem.get_emission -> EmissionModule (возврат в x0) */
typedef void  (*em_set_enabled_fn)(void*, int, const void*); /* EmissionModule.set_enabled */
typedef void  (*ps_emit5_fn)(void*, Vec3, Vec3, float, float, Color32, const void*); /* ParticleSystem.Emit(pos, vel, size, life, color32) */                  /* static Object.FindObjectsOfTypeAll(Type) */
typedef void  (*light_set_color_fn)(void*, Color, const void*);         /* Light.set_color */
typedef Color (*light_get_color_fn)(void*, const void*);
typedef void  (*static_destroy_fn)(void*, const void*);      /* static Object.Destroy(Object) */
typedef void* (*mat_get_texture_fn)(void*, void*, const void*);   /* Material.GetTexture(string) / get_mainTexture */                  /* Light.get_color */

static const BeariteApi* g_api;
static const char* g_id;

/* доп. экспорты il2cpp (в шаблонном BeariteIl2cpp их нет) */
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
static void* mi_shader_find;
static void* mi_object_instantiate;
static void* mi_mat_setshader;
static void* mi_trail_setmaterial;
static void* mi_trail_getmaterial;
static void* mi_trail_mvd;         /* TrailRenderer.set_minVertexDistance */
static void* mi_time_scale;        /* Time.set_timeScale (static) */
static void* mi_tr_get_position;   /* Transform.get_position */
static void* mi_set_ambmode;       /* RenderSettings.set_ambientMode */
static void* mi_get_skybox;        /* RenderSettings.get_skybox (static) */
static void* mi_mat_setcolor2;     /* Material.SetColor(string, Color) */
static void* mi_mat_getcolor;      /* Material.GetColor(string) */
static void* mi_mat_hasprop;       /* Material.HasProperty(string) */
static void* mi_tr_set_position;   /* Transform.set_position */
static void* mi_tr_set_rotation;   /* Transform.set_rotation */
static void* mi_set_enabled;       /* Behaviour.set_enabled */
static void* mi_go_ctor;           /* GameObject..ctor(string) */
static void* g_go_klass;           /* Il2CppClass GameObject */
static alloc_obj_fn fn_obj_new;    /* il2cpp_object_new */
static void* (*il_class_get_nested)(void*, void**);  /* il2cpp_class_get_nested_types */
static unsigned (*il_gchandle_new)(void*, int);      /* il2cpp_gchandle_new(obj, pinned) -> handle */
static void* (*il_array_new)(void*, int);          /* il2cpp_array_new(arrayKlass, size) */
static void* (*il_array_class_get)(void*, unsigned); /* il2cpp_array_class_get(elementKlass, rank) -> klass T[] */
static const char* (*il_class_get_name)(void*);      /* il2cpp_class_get_name */
static void* g_ps_type;            /* System.Type ParticleSystem */
static void* g_psrend_type;        /* System.Type ParticleSystemRenderer */
static void* g_skych_type;         /* System.Type SkyboxChanger */
static void* g_skytrig_type;       /* System.Type CustomSkyboxTrigger */
static void* g_skyeff_type;        /* System.Type CustomSkyboxEffects */
static void* g_meshrend_type;      /* System.Type MeshRenderer */
static void* g_skinrend_type;      /* System.Type SkinnedMeshRenderer */
static void* g_sprend_type;        /* System.Type SpriteRenderer */
static void* mi_obj_getname;       /* Object.get_name */
static get_obj_fn fn_obj_getname;
static int g_sky3_diag;
static void* mi_tr_childcount;     /* Transform.get_childCount */
static void* mi_tr_getchild;       /* Transform.GetChild(int) */
static void* mi_mat_enablekw;      /* Material.EnableKeyword(string) */
static void* mi_mat_getshader;     /* Material.get_shader */
static void* mi_shader_getname;    /* Shader.get_name */
static const char* (*il_string_chars)(void*);   /* il2cpp_string_chars */
static int         (*il_string_length)(void*);  /* il2cpp_string_length */
static get_obj_fn  fn_mat_getshader;
static get_obj_fn  fn_shader_getname;
static int   (*fn_tr_childcount)(void*, const void*);
static void* (*fn_tr_getchild)(void*, int, const void*);
static void  (*fn_mat_enablekw)(void*, void*, const void*);
static int   g_sky3_logged;
typedef struct { void* mat; Color orig[8]; unsigned saved; } Sky3Mat;
static Sky3Mat g_sky3_mats[32];
static void* mi_skych_color;       /* SkyboxChanger.ChangeSkyboxColor (private!) */
static void  (*fn_skych_color)(void*, Color, const void*);
static int   g_skych_logged;
typedef struct { void* ptr; Color orig; } SkyChOrig;
static SkyChOrig g_skych_orig[8];
static void* g_soft_mat;           /* украденный мягкий партикл-материал (белый) */
static void* g_sh_particles;       /* Shader Particles/Standard Unlit (лениво) */
static int   g_soft_tried;
static void* g_em_klass;           /* Il2CppClass ParticleSystem/EmissionModule */
static void* mi_tex_ctor;          /* Texture2D..ctor(w,h,format,mipChain) */
static void* mi_tex_loadimage;     /* ImageConversion.LoadImage(tex, byte[]) */
static void* g_byte_klass;         /* Il2CppClass System.Byte */
static void (*fn_tex_ctor)(void*, int, int, int, int, const void*);
static int  (*fn_tex_loadimage)(void*, void*, const void*);
static void* mi_mat_getmaintex;    /* Material.get_mainTexture */
static void* mi_mat_gettexture;    /* Material.GetTexture(string) */
static get_obj_fn       fn_mat_getmaintex;   /* (self, mi) */
static mat_get_texture_fn fn_mat_gettexture; /* (self, name, mi) */
static void* mi_mat_getcolor_prop; /* Material.get_color (property) */
static void* mi_object_destroy;    /* Object.Destroy */
static Color (*fn_mat_getcolor_prop)(void*, const void*);   /* Material.get_color property */
static static_destroy_fn fn_object_destroy;
static void* mi_tmp_settext;       /* TMPro.TMP_Text.set_text */
static void* mi_tm_settext;        /* UnityEngine.TextMesh.set_text */
static void* mi_tr_get_rotation;   /* Transform.get_rotation */
static tmp_set_text_fn  fn_tm_settext;
static get_rot_fn       fn_tr_get_rotation;

static void* mi_tmp_setcolor;      /* TMPro.TMP_Text.set_color */
static tmp_set_text_fn  fn_tmp_settext;
static tmp_set_color_fn fn_tmp_setcolor;
static void* mi_ps_getemission;    /* ParticleSystem.get_emission */
static void* mi_ps_emit5;          /* ParticleSystem.Emit(argc 5) */
static void* mi_em_enabled;        /* EmissionModule.set_enabled */
static ps_get_emission_fn fn_ps_getemission;
static ps_emit5_fn        fn_ps_emit5;
static em_set_enabled_fn  fn_em_set_enabled;
static void* g_sun_go;
static void* g_sun_ps;
static float g_sun_next_emit;
static unsigned g_hnd_orbit[8];
static void* g_amb_go;
static void* g_amb_ps;
static float g_amb_next_emit;
static unsigned g_amb_rng = 12345u;
static void* mi_go_setactive;      /* GameObject.SetActive(bool) */
static void* mi_tr_set_parent;     /* Transform.set_parent */
static void* mi_cam_replshader;    /* Camera.SetReplacementShader */
static void* mi_cam_resetshader;   /* Camera.ResetReplacementShader */
static void* mi_set_clearflags;    /* Camera.set_clearFlags */
static void* mi_set_bgcolor;       /* Camera.set_backgroundColor */
static void* mi_find_objects;      /* Object.FindObjectsOfTypeAll */
static void* mi_light_color;       /* Light.set_color */
static void* mi_light_getcolor;    /* Light.get_color */
static void* mi_set_fog;           /* RenderSettings.set_fog */
static void* g_light_type;         /* System.Type Light */
static void* g_brain_type;         /* System.Type CinemachineBrain */

static set_color_fn    fn_set_color;
static get_obj_fn      fn_get_gameobject;
static go_type_fn      fn_go_getcomponent;
static go_type_fn      fn_go_addcomponent;
static set_float_fn    fn_trail_time;
static set_float_fn    fn_trail_startwidth;
static set_float_fn    fn_trail_endwidth;
static set_color_fn    fn_trail_startcolor;
static set_color_fn    fn_trail_endcolor;
static shader_find_fn   fn_shader_find;
static instantiate_fn   fn_object_instantiate;
static set_shader_fn    fn_mat_setshader;
static set_material_fn  fn_trail_setmaterial;
static get_obj_fn       fn_trail_getmaterial;
static set_float_fn     fn_trail_mvd;
static static_float_fn  fn_time_scale;
static get_pos_fn       fn_tr_get_position;
static static_int_fn    fn_set_ambmode;
static int g_ambmode_changed;  /* мы переключили ambientMode со Skybox на Trilight */
static static_obj_fn    fn_get_skybox;
static mat_setcolor2_fn fn_mat_setcolor2;
static mat_getcolor_fn  fn_mat_getcolor;
static mat_hasprop_fn   fn_mat_hasprop;
static set_pos_fn       fn_tr_set_position;
static set_rot_fn       fn_tr_set_rotation;
static set_enabled_fn   fn_set_enabled;
static go_ctor_fn       fn_go_ctor;
static go_setactive_fn  fn_go_setactive;
static set_parent_fn    fn_tr_set_parent;
static cam_replshader_fn fn_cam_replshader;
static cam_resetshader_fn fn_cam_resetshader;
static set_bgcolor_fn   fn_cam_set_bgcolor;
static set_clearflags_fn fn_cam_set_clearflags;
static find_objects_fn   fn_find_objects;
static light_set_color_fn fn_light_color;
static light_get_color_fn fn_light_getcolor;
static static_bool_fn    fn_set_fog;
static void* g_sh_flat;            /* Shader Sprites/Default */
static void* g_sh_unlit;           /* Shader Unlit/Texture */
static void* g_skybox;         /* текущий материал скайбокса */
static int   g_skybox_tinted;  /* мы сейчас красим скайбокс */

/* v1.3.2: свойства скайбокса, которые пробуем красить (orig = исходный цвет) */
typedef struct { const char* prop; Color orig; int has; } SkyTintProp;
static SkyTintProp g_tint_props[] = {
    { "_SkyTint",     {0,0,0,0}, 0 },   /* Procedural skybox */
    { "_GroundColor", {0,0,0,0}, 0 },   /* Procedural skybox */
    { "_Tint",        {0,0,0,0}, 0 },   /* Classic 6-sided skybox */
    { "_Color",       {0,0,0,0}, 0 },   /* Generic */
    { "_BaseColor",   {0,0,0,0}, 0 },   /* URP-ish */
};

static void* g_trail_type;
static void* g_trail;
static void* g_trail_last;   /* предыдущий объект трейла (детект замены/пересоздания) */
static int   g_trail_mat_done;
static long  g_tick;
static float g_hue;
static float g_spin_angle;
static float g_disco_hue;
static float g_jelly_t;
static int   g_head_scaled;
static int   g_dbg_chroma;
static int   g_jelly_parts;
static Color g_skin_orig[2];   /* исходные цвета материалов скина для восстановления */
static int   g_chroma_saved;   /* битмаск частей, которые сейчас в желе (бит0 тело, бит1 голова, бит2 глаза) */

static void* mi_tr_set_euler;
static void* mi_tr_set_scale;
static void* mi_set_sky;
static void* mi_set_equator;
static void* mi_set_ground;
static void* mi_tr_get_euler;
static void* mi_trail_corners;
static void* mi_trail_caps;
static void* mi_mat_setmaintex;
static void (*orig_set_aspect)(void*, float, const void*);
static void* mi_cam_aspect;
static void* mi_cam_main;
static void* mi_get_transform;
static void* mi_tr_get_scale;

static static_obj_fn fn_cam_main;
static get_obj_fn    fn_get_transform;
static get_vec3_fn   fn_tr_get_scale;
static tr_vec3_fn   fn_tr_set_euler;
static tr_vec3_fn   fn_tr_set_scale;
static set_ambient_fn fn_set_sky;
static set_ambient_fn fn_set_equator;
static set_ambient_fn fn_set_ground;
static get_euler_fn  fn_tr_get_euler;
static set_int_fn    fn_trail_corners;
static set_int_fn    fn_trail_caps;
static set_material_fn fn_mat_setmaintex;

static Vec3 g_base_euler;
static int  g_base_valid;
static float g_pulse_t;
static int   g_ts_applied;   /* timeScale сейчас изменён нами (вернуть 1.0 при выключении) */
static float g_speed;        /* сглаженная скорость локального игрока (ед./с) для динамического FOV */
static Vec3  g_last_pos;
static int   g_last_pos_valid;
static float g_bear_base_scale = 1.f;  /* исходный localScale контроллера */
static int   g_bear_scaled;            /* масштаб контроллера сейчас изменён нами */
static float g_time_t;                 /* фаза для пульса времени */
static float g_cine_angle;             /* угол орбиты кинокамеры */
static void* g_brain;                  /* CinemachineBrain (для restore) */
static int   g_brain_disabled;
static int   g_cine_warned;
static Vec3  g_cam_pos;                /* сглаженная позиция камеры (smooth mode) */
static int   g_cam_pos_valid;
static int   g_grade_applied = -1;     /* какой color_grade сейчас применён (-1 none) */
static void* g_orbit_tr[8];            /* TrailRenderer орбитальных спутников */
static void* g_orbit_go[8];
static int   g_orbit_created;          /* сколько спутников создано */
static int   g_orbit_on;               /* сколько сейчас активно */
static void* g_lights_arr;             /* Object[] Light'ов сцены */
static float g_lights_fetched;
static int   g_lights_tinted;
static int   g_fog_on;
static float g_eyes_base_scale = 1.f;
static int   g_eyes_scaled;
typedef struct { void* ptr; Color orig; } LightOrig;
static LightOrig g_light_orig[24];

/* v1.10.4: проверка "жив ли Unity-объект" (m_CachedPtr @ 0x10).
   После смены сцены объект уничтожается, а указатель остаётся -
   без этой проверки Emit по трупу = SIGSEGV. */
static int obj_alive(void* obj) {
    if (!obj) return 0;
    return *(void**)((char*)obj + 0x10) != 0;
}

/* v1.10.2: объекты из il2cpp_object_new не имеют корней GC - Unity их
   собирает за ~40 сек, указатель становится мусором (был SIGSEGV в visuals_tick).
   Закрепляем GC-handle'ом на всю жизнь мода. */
static unsigned gc_pin(void* obj) {
    if (!obj || !il_gchandle_new) return 0;
    return il_gchandle_new(obj, 1);
}

static Color hsv2rgb(float h) {
    h -= (float)(int)h;                 /* защита от h >= 1 (disco hue + сдвиги) */
    if (h < 0) h += 1.f;
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
    return (Color){0.45f, 0.85f, 1.f, 1.f};   /* bubbles */
}

/* пресеты неба v1.3: небо/экватор/земля */
typedef struct { const char* name; Color sky, equ, grd; } SkyPreset;
static const SkyPreset kSkyPresets[] = {
    { "off",    {0,0,0,0},              {0,0,0,0},              {0,0,0,0}              },
    { "sunset", {1.f,0.45f,0.20f,1.f},  {1.f,0.20f,0.35f,1.f},  {0.30f,0.05f,0.10f,1.f} },
    { "night",  {0.02f,0.03f,0.09f,1.f},{0.05f,0.04f,0.14f,1.f},{0.01f,0.01f,0.03f,1.f} },
    { "blood",  {0.55f,0.02f,0.02f,1.f},{0.30f,0.01f,0.05f,1.f},{0.12f,0.0f,0.02f,1.f}  },
    { "ocean",  {0.15f,0.55f,0.75f,1.f},{0.05f,0.30f,0.50f,1.f},{0.02f,0.12f,0.20f,1.f} },
    { "mint",   {0.45f,1.f,0.70f,1.f},  {0.20f,0.70f,0.45f,1.f},{0.08f,0.28f,0.18f,1.f} },
};
static const int kSkyPresetCount = sizeof kSkyPresets / sizeof kSkyPresets[0];


/* ================= "Пакет безумия" ================= */

static int g_dbg_crazy_once, g_dbg_head, g_dbg_spin, g_dbg_jelly, g_dbg_disco;

static void crazy_tick(void* player, void* cosmetics, float dt) {
    double spin_spd = g_api->get_float(g_id, "spin_speed", 6.0);

    void* head   = cosmetics ? *(void**)((char*)cosmetics + 0x38) : 0;
    void* body   = cosmetics ? *(void**)((char*)cosmetics + 0x40) : 0;
    void* shicka = cosmetics ? *(void**)((char*)cosmetics + 0x48) : 0;

    if (!g_dbg_crazy_once) {
        char b[224];
        snprintf(b, sizeof b, "FishVisuals: CRAZY ptrs cosmetics=%p head=%p body=%p shicka=%p",
                 cosmetics, head, body, shicka);
        g_dbg_crazy_once = 1;
        g_api->log(g_id, b);
    }

    /* БОЛЬШАЯ ГОЛОВА */
    int big_head = g_api->get_bool(g_id, "big_head", 0) || g_api->get_bool(g_id, "selftest", 0);
    if (big_head && head && fn_tr_set_scale) {
        float s = (float)g_api->get_float(g_id, "head_scale", 2.5f);
        fn_tr_set_scale(head, (Vec3){s, s, s}, mi_tr_set_scale);
        g_head_scaled = 1;
        if (!g_dbg_head && fn_tr_get_scale) {
            g_dbg_head = 1;
            Vec3 rb = fn_tr_get_scale(head, mi_tr_get_scale);
            char b[160];
            snprintf(b, sizeof b, "FishVisuals: BIG_HEAD применён, readback scale=(%.2f,%.2f,%.2f)",
                     rb.x, rb.y, rb.z);
            g_api->log(g_id, b);
        }
    } else if (!big_head && g_head_scaled && head && fn_tr_set_scale) {
        fn_tr_set_scale(head, (Vec3){1, 1, 1}, mi_tr_set_scale);
        g_head_scaled = 0;
    }

    /* ГОДЗИЛЛА-МЕДВЕДЬ: удалена в v1.7 (bear_scale всегда 1.0) */
    float bear_scale = 1.0f;
    void* ctrl_tr2 = fn_get_transform ? fn_get_transform(player, mi_get_transform) : 0;
    if (ctrl_tr2 && fn_tr_get_scale && fn_tr_set_scale) {
        if (!g_bear_scaled && bear_scale != 1.f) {
            Vec3 cur = fn_tr_get_scale(ctrl_tr2, mi_tr_get_scale);
            g_bear_base_scale = (cur.x > 0.f) ? cur.x : 1.f;
            float s = g_bear_base_scale * bear_scale;
            fn_tr_set_scale(ctrl_tr2, (Vec3){s, s, s}, mi_tr_set_scale);
            g_bear_scaled = 1;
        } else if (g_bear_scaled && bear_scale == 1.f) {
            fn_tr_set_scale(ctrl_tr2, (Vec3){g_bear_base_scale, g_bear_base_scale, g_bear_base_scale}, mi_tr_set_scale);
            g_bear_scaled = 0;
        } else if (g_bear_scaled && bear_scale != 1.f) {
            float s = g_bear_base_scale * bear_scale;
            fn_tr_set_scale(ctrl_tr2, (Vec3){s, s, s}, mi_tr_set_scale);
        }
    }

    /* БОЛЬШИЕ ГЛАЗА: удалены в v1.7 */
    int big_eyes = 0;
    void* eyes = cosmetics ? *(void**)((char*)cosmetics + 0x50) : 0;
    if (big_eyes && eyes && fn_tr_set_scale && fn_tr_get_scale) {
        if (!g_eyes_scaled) {
            Vec3 cur = fn_tr_get_scale(eyes, mi_tr_get_scale);
            g_eyes_base_scale = (cur.x > 0.f) ? cur.x : 1.f;
            g_eyes_scaled = 1;
        }
        float es = g_eyes_base_scale * 2.0f;   /* eyes_scale удалена из json */
        fn_tr_set_scale(eyes, (Vec3){es, es, es}, mi_tr_set_scale);
    } else if (!big_eyes && g_eyes_scaled && eyes && fn_tr_set_scale) {
        fn_tr_set_scale(eyes, (Vec3){g_eyes_base_scale, g_eyes_base_scale, g_eyes_base_scale}, mi_tr_set_scale);
        g_eyes_scaled = 0;
    }

    /* ВРАЩЕНИЯ: крутим transform контроллера (аниматор его не трогает) */
    void* crc  = cosmetics ? *(void**)((char*)cosmetics + 0x58) : 0;
    void* root = crc ? *(void**)((char*)crc + 0x20) : 0;
    void* ctrl_tr = fn_get_transform ? fn_get_transform(player, mi_get_transform) : 0;

    int body_on   = 0;   /* v1.7: body_spin удалён по запросу */
    int exor_on   = 0;   /* v1.7: head_spin (экзорцист) удалён по запросу */
    int shicka_on = g_api->get_bool(g_id, "shicka_copter", 0);
    int any_spin  = body_on | exor_on | shicka_on;

    if (any_spin) {
        g_spin_angle += (float)(6.0 * spin_spd) * dt;

        if (body_on && ctrl_tr && fn_tr_set_euler && fn_tr_get_euler) {
            if (!g_base_valid) {
                g_base_euler = fn_tr_get_euler(ctrl_tr, mi_tr_get_euler);
                g_base_valid = 1;
            }
            Vec3 e = g_base_euler;
            e.y += g_spin_angle;
            e.z += (float)g_api->get_float(g_id, "world_roll", 0.0);
            fn_tr_set_euler(ctrl_tr, e, mi_tr_set_euler);
            if (g_dbg_spin == 0) { g_dbg_spin = 1;
                Vec3 rb = fn_tr_get_euler(ctrl_tr, mi_tr_get_euler);
                char b[128]; snprintf(b, sizeof b, "FishVisuals: SPIN readback ctrl euler=(%.1f,%.1f,%.1f)", rb.x, rb.y, rb.z);
                g_api->log(g_id, b);
            }
        }

        if (exor_on && root && fn_tr_set_euler)
            fn_tr_set_euler(root, (Vec3){g_spin_angle * 1.2f, 0, 0}, mi_tr_set_euler);

        if (shicka_on && shicka && fn_tr_set_euler)
            fn_tr_set_euler(shicka, (Vec3){0, g_spin_angle * 3.0f, 0}, mi_tr_set_euler);
    } else if (g_base_valid && ctrl_tr && fn_tr_set_euler) {
        Vec3 e = g_base_euler;
        e.z += (float)g_api->get_float(g_id, "world_roll", 0.0);   /* КРЕН МИРА v1.6 */
        fn_tr_set_euler(ctrl_tr, e, mi_tr_set_euler);
        if ((float)g_api->get_float(g_id, "world_roll", 0.0) == 0.f) {
            g_base_valid = 0;
            g_spin_angle = 0.f;
        }
    }

    /* РЕЗИНОВЫЙ МЕДВЕДЬ v1.9.2 - волна по всем частям тела.
       Каждая часть сжимается со своей фазой: волна бежит тело -> голова -> глаза. */
    {
        int jelly_on = g_api->get_bool(g_id, "jelly", 0);
        float amp = (float)g_api->get_float(g_id, "jelly_strength", 0.25);
        if (amp < 0.02f) amp = 0.02f;
        if (jelly_on && fn_tr_set_scale) {
            g_jelly_t += 10.f * dt;
            /* v1.11.2: корень модели (crc@0x58 -> characterRootTransform@0x20) -
               плющит ВСЕГО медведя включая руки и ноги (bodyTransform их не содержит) */
            void* crc = cosmetics ? *(void**)((char*)cosmetics + 0x58) : 0;
            void* root = crc ? *(void**)((char*)crc + 0x20) : 0;
            void* squash = root ? root : body;   /* запасной вариант - старое тело */
            if (squash) {
                float p = sinf(g_jelly_t) * amp;
                fn_tr_set_scale(squash, (Vec3){1.f - p, 1.f + p * 1.3f, 1.f - p}, mi_tr_set_scale);
                g_jelly_parts |= 1;
            }
            /* голова (фаза +2.1) - только если не включена большая голова (та сама управляет) */
            if (head && !g_api->get_bool(g_id, "big_head", 0)) {
                float p = sinf(g_jelly_t + 2.1f) * amp * 0.8f;
                fn_tr_set_scale(head, (Vec3){1.f - p, 1.f + p * 1.25f, 1.f - p}, mi_tr_set_scale);
                g_jelly_parts |= 2;
            }
            /* глаза (фаза +4.2) */
            void* eyes = cosmetics ? *(void**)((char*)cosmetics + 0x50) : 0;
            if (eyes) {
                float p = sinf(g_jelly_t + 4.2f) * amp * 0.6f;
                fn_tr_set_scale(eyes, (Vec3){1.f - p, 1.f + p * 1.2f, 1.f - p}, mi_tr_set_scale);
                g_jelly_parts |= 4;
            }
            if (!g_dbg_jelly) { g_dbg_jelly = 1; g_api->log(g_id, "FishVisuals: JELLY FULL тикает"); }
        } else if (g_jelly_parts) {
            /* выключили - возвращаем частям scale 1 один раз */
            void* crc0 = cosmetics ? *(void**)((char*)cosmetics + 0x58) : 0;
            void* root0 = crc0 ? *(void**)((char*)crc0 + 0x20) : 0;
            void* squash0 = root0 ? root0 : body;
            if ((g_jelly_parts & 1) && squash0)
                fn_tr_set_scale(squash0, (Vec3){1, 1, 1}, mi_tr_set_scale);
            if ((g_jelly_parts & 2) && head && !g_api->get_bool(g_id, "big_head", 0))
                fn_tr_set_scale(head, (Vec3){1, 1, 1}, mi_tr_set_scale);
            if ((g_jelly_parts & 4) && eyes)
                fn_tr_set_scale(eyes, (Vec3){1, 1, 1}, mi_tr_set_scale);
            g_jelly_parts = 0;
        }
    }

    /* ПРЕСЕТЫ НЕБА v1.3: статичный цветовой набор с регулируемой яркостью.
       Имеет приоритет над диско-небом (если пресет != off, диско пропускается). */
    /* v1.3.1: без этого фикса небо не работает. В SBA ambientMode = Skybox,
       и игнорируются ВСЕ записи в ambient*Color. Переключаемся на Trilight,
       когда активен любой небесный эффект, и возвращаем Skybox, когда все выключены. */
    int disco_pre  = g_api->get_bool(g_id, "disco_sky", 0);
    int pulse_pre  = g_api->get_bool(g_id, "sky_pulse", 0);
    int beauty_pre = g_api->get_bool(g_id, "beauty_mode", 0);
    int preset_pre = strcmp(g_api->get_string(g_id, "sky_preset", "off"), "off") != 0;
    int sky_any = disco_pre | pulse_pre | beauty_pre | preset_pre;
    if (fn_set_ambmode) {
        if (sky_any && !g_ambmode_changed) {
            fn_set_ambmode(1, mi_set_ambmode);   /* AmbientMode.Trilight */
            g_ambmode_changed = 1;
            g_api->log(g_id, "FishVisuals: ambientMode -> Trilight (небо оживёт)");
        } else if (!sky_any && g_ambmode_changed) {
            fn_set_ambmode(0, mi_set_ambmode);   /* AmbientMode.Skybox */
            g_ambmode_changed = 0;
        }
    }

    Color sky_tint = {1.f, 1.f, 1.f, 1.f};   /* v1.3.2: цвет для скайбокса */

    const char* preset_name = g_api->get_string(g_id, "sky_preset", "off");
    int preset_idx = 0;
    for (int i = 1; i < kSkyPresetCount; i++)
        if (!strcmp(preset_name, kSkyPresets[i].name)) { preset_idx = i; break; }
    int preset_active = (preset_idx != 0) || g_api->get_bool(g_id, "beauty_mode", 0);
    if (preset_active && fn_set_sky) {
        if (preset_idx == 0) preset_idx = 5;   /* beauty_mode без пресета = mint */
        const SkyPreset* p = &kSkyPresets[preset_idx];
        float k = (float)g_api->get_float(g_id, "sky_intensity", 1.0);
        if (k < 0.f) k = 0.f;
        Color sky = { p->sky.r*k, p->sky.g*k, p->sky.b*k, 1.f };
        if (fn_set_equator) { Color e = { p->equ.r*k, p->equ.g*k, p->equ.b*k, 1.f }; fn_set_equator(e, mi_set_equator); }
        if (fn_set_ground)  { Color gr = { p->grd.r*k, p->grd.g*k, p->grd.b*k, 1.f }; fn_set_ground(gr, mi_set_ground); }
        fn_set_sky(sky, mi_set_sky);
        sky_tint = sky;
    }

    /* ДИСКО-НЕБО + ПУЛЬС */
    int disco_on = g_api->get_bool(g_id, "disco_sky", 0) && !preset_active;
    int pulse_on = g_api->get_bool(g_id, "sky_pulse", 0);
    int beauty_sky = g_api->get_bool(g_id, "beauty_mode", 0);
    if ((disco_on || pulse_on || beauty_sky) && fn_set_sky) {
        double spd = g_api->get_float(g_id, "disco_speed", 2.0);
        g_disco_hue += 0.15f * (float)spd * dt;
        if (g_disco_hue > 1.f) g_disco_hue -= 1.f;
        g_pulse_t += dt;
        float br = pulse_on ? (1.f + 0.3f * sinf(g_pulse_t * 2.f)) : 1.f;
        Color sky = hsv2rgb(g_disco_hue);
        sky.r *= br; sky.g *= br; sky.b *= br;
        if (fn_set_equator) { Color e = hsv2rgb(g_disco_hue + 0.08f); e.r*=br; e.g*=br; e.b*=br; fn_set_equator(e, mi_set_equator); }
        if (fn_set_ground)  { Color gr = hsv2rgb(g_disco_hue + 0.16f); gr.r*=br; gr.g*=br; gr.b*=br; fn_set_ground(gr, mi_set_ground); }
        fn_set_sky(sky, mi_set_sky);
        sky_tint = sky;
        if (!g_dbg_disco) { g_dbg_disco = 1; g_api->log(g_id, "FishVisuals: DISCO тикает"); }
    }

    /* v1.3.2: красим САМ скайбокс. ambient*Color влияет только на освещение
       объектов - фон неба рисует отдельный материал (RenderSettings.skybox).
       Пробуем все известные свойства цвета; исходные значения запоминаем
       и восстанавливаем при выключении. */
    if (sky_any) {
        if (fn_get_skybox && fn_mat_hasprop && fn_mat_setcolor2) {
            void* sb = fn_get_skybox(mi_get_skybox);
            if (sb && sb != g_skybox) { g_skybox = sb; g_skybox_tinted = 0; }
            if (sb) {
                /* v1.16: без HasProperty (битый резолв перегрузок) - пишем всех кандидатов */
                for (int i = 0; i < (int)(sizeof g_tint_props / sizeof g_tint_props[0]); i++) {
                    void* nm = bearite_il2.string_new(g_tint_props[i].prop);
                    if (!nm) continue;
                    if (!g_tint_props[i].has && fn_mat_getcolor)
                        g_tint_props[i].orig = fn_mat_getcolor(sb, nm, mi_mat_getcolor);
                    g_tint_props[i].has = 1;
                    fn_mat_setcolor2(sb, nm, sky_tint, mi_mat_setcolor2);
                }
                if (!g_skybox_tinted) {
                    g_skybox_tinted = 1;
                    g_api->log(g_id, "FishVisuals: skybox tinted");
                }
            }
        }
    } else if (g_skybox_tinted && g_skybox && fn_mat_setcolor2) {
        for (int i = 0; i < (int)(sizeof g_tint_props / sizeof g_tint_props[0]); i++) {
            if (!g_tint_props[i].has) continue;
            void* nm = bearite_il2.string_new(g_tint_props[i].prop);
            if (nm) fn_mat_setcolor2(g_skybox, nm, g_tint_props[i].orig, mi_mat_setcolor2);
            g_tint_props[i].has = 0;
        }
        g_skybox_tinted = 0;
    }
}

static const char* kShaderCandidates[] = {
    "Particles/Standard Unlit",
    "Legacy Shaders/Particles/Alpha Blended",
    "Sprites/Default",
};

static void* make_trail_material(void* cosmetics) {
    void* skin = *(void**)((char*)cosmetics + 0x20);
    if (!skin || !fn_object_instantiate) return skin;
    void* mat = fn_object_instantiate(skin, mi_object_instantiate);
    if (mat && fn_shader_find && fn_mat_setshader) {
        void* sh = 0; const char* used = 0;
        for (int i = 0; i < 3 && !sh; i++) {
            void* nm = bearite_il2.string_new(kShaderCandidates[i]);
            sh = nm ? fn_shader_find(nm, mi_shader_find) : 0;
            if (sh) used = kShaderCandidates[i];
        }
        if (sh) {
            fn_mat_setshader(mat, sh, mi_mat_setshader);
            if (fn_mat_setmaintex) fn_mat_setmaintex(mat, 0, mi_mat_setmaintex);
            char b[128];
            snprintf(b, sizeof b, "FishVisuals: trail shader = %s", used);
            g_api->log(g_id, b);
        } else {
            g_api->log(g_id, "FishVisuals: no particle shader found, trail uses skin clone");
        }
    }
    return mat ? mat : skin;
}

/* ФИКС v1.2: компонент переполучаем КАЖДЫЙ тик (GetComponent дёшев).
   Старый вариант хранил указатель 256 тиков - после уничтожения объекта
   (смена комнаты/уровня) это dangling pointer = краш. Заодно детектим
   замену объекта и переназначаем материал (иначе трейл снова маджентовый). */
static void*   g_trail_go;        /* GO игрока, к которому привязан трейл */
static float   g_trail_add_cd;    /* кулдаун повторного AddComponent */

static void ensure_trail(void* player) {
    if (!fn_get_gameobject || !fn_go_getcomponent || !g_trail_type) return;
    void* go = fn_get_gameobject(player, mi_get_gameobject);
    if (!go) { g_trail = 0; g_trail_last = 0; g_trail_go = 0; return; }
    if (g_trail_go && go != g_trail_go) return;   /* чужой объект - не трогаем */
    void* tr = fn_go_getcomponent(go, g_trail_type, mi_go_getcomponent);
    if (!tr && fn_go_addcomponent && g_time_t >= g_trail_add_cd) {
        tr = fn_go_addcomponent(go, g_trail_type, mi_go_addcomponent);
        g_trail_add_cd = g_time_t + 2.f;   /* не чаще раза в 2 секунды */
    }
    if (tr != g_trail_last) {
        g_trail = tr;
        g_trail_last = tr;
        g_trail_go = tr ? go : 0;
        g_trail_mat_done = 0;
        if (g_trail && go == g_trail_go)
            g_api->log(g_id, "FishVisuals: трейл получен/пересоздан");
    }
}

/* ================= КИНОКАМЕРА (v1.4) =================
   Орбитальная камера вокруг игрока. Отключаем CinemachineBrain (иначе он
   перезаписывает transform камеры в LateUpdate после нашего тика) и сами
   крутим Camera.main по окружности с look-at на игрока. */
/* ================= КИНОКАМЕРА / ПЛАВНАЯ КАМЕРА (v1.5) =================
   Режимы камеры (cam_mode):
   - orbit : орбита вокруг игрока (облёты)
   - smooth: плавное следование за игроком с экспоненциальным сглаживанием
             (как SmoothCamera в читах на MC)
   В обоих режимах отключаем CinemachineBrain (иначе он перезаписывает
   transform камеры в LateUpdate после нашего тика). */
static void cam_set_brain(int disable) {
    if (disable && !g_brain_disabled && fn_go_getcomponent && g_brain_type && fn_set_enabled && fn_cam_main) {
        void* cam = fn_cam_main(mi_cam_main);
        void* go = cam ? fn_get_gameobject(cam, mi_get_gameobject) : 0;
        void* brain = go ? fn_go_getcomponent(go, g_brain_type, mi_go_getcomponent) : 0;
        if (brain) {
            fn_set_enabled(brain, 0, mi_set_enabled);
            g_brain = brain;
            g_brain_disabled = 1;
            g_api->log(g_id, "FishVisuals: CINEMA - CinemachineBrain отключён");
        } else if (!g_cine_warned) {
            g_cine_warned = 1;
            g_api->log(g_id, "FishVisuals: CINEMA - brain не найден на камере");
        }
    } else if (!disable && g_brain_disabled && g_brain && fn_set_enabled) {
        fn_set_enabled(g_brain, 1, mi_set_enabled);
        g_brain_disabled = 0;
        g_brain = 0;
        g_cam_pos_valid = 0;
        g_api->log(g_id, "FishVisuals: CINEMA - CinemachineBrain включён");
    }
}

/* look-at кватернион из направления камеры -> точки */
static Quat quat_lookat(Vec3 from, Vec3 to) {
    float dx = to.x - from.x, dy = to.y - from.y, dz = to.z - from.z;
    float yaw = atan2f(dx, dz);
    float hor = sqrtf(dx*dx + dz*dz);
    /* v1.7: знак pitch исправлен (было: камера смотрела в небо, а не на игрока) */
    float pitch = atan2f(dy, (hor > 0.001f) ? hor : 0.001f);
    float hy = yaw * 0.5f, hp = pitch * 0.5f;
    Quat q;
    q.x = cosf(hy) * sinf(hp);
    q.y = sinf(hy) * cosf(hp);
    q.z = -sinf(hy) * sinf(hp);
    q.w = cosf(hy) * cosf(hp);
    return q;
}

static void cine_tick(float dt) {
    const char* mode_s = g_api->get_string(g_id, "cam_mode", "off");
    int mode = 0;
    if (!strcmp(mode_s, "orbit"))  mode = 1;
    if (!strcmp(mode_s, "smooth")) mode = 2;
    if (!strcmp(mode_s, "retro"))  mode = 3;
    if (mode == 0) { cam_set_brain(0); return; }
    if (!fn_cam_main || !fn_get_transform) return;
    cam_set_brain(1);
    if (!g_brain_disabled) return;   /* brain не отключился - не лезем к камере */
    void* cam = fn_cam_main(mi_cam_main);
    void* cam_tr = cam ? fn_get_transform(cam, mi_get_transform) : 0;
    if (!cam_tr || !fn_tr_set_position || !fn_tr_set_rotation || !g_last_pos_valid) return;

    Vec3 p = g_last_pos;
    if (mode == 1) {   /* орбита */
        double spd = g_api->get_float(g_id, "cine_speed", 0.6);
        float r = (float)g_api->get_float(g_id, "cine_radius", 6.0);
        float h = (float)g_api->get_float(g_id, "cine_height", 2.5);
        g_cine_angle += (float)spd * dt;
        Vec3 pos = { p.x + r * cosf(g_cine_angle), p.y + h, p.z + r * sinf(g_cine_angle) };
        float sh = (float)g_api->get_float(g_id, "cam_shake", 0.0);
        if (sh > 0.f) {
            pos.x += sh * 0.05f * sinf(g_time_t * 43.f);
            pos.y += sh * 0.05f * sinf(g_time_t * 57.f + 1.f);
        }
        fn_tr_set_position(cam_tr, pos, mi_tr_set_position);
        fn_tr_set_rotation(cam_tr, quat_lookat(pos, p), mi_tr_set_rotation);
    } else if (mode == 3) {   /* РЕТРО-КАМЕРА: как в старых версиях SBA -
       фиксированный угол, камера НЕ поворачивается за медведем,
       только плавно следует за позицией */
        double lerp = g_api->get_float(g_id, "cam_lerp", 6.0);
        if (lerp < 0.1) lerp = 0.1;
        float dist = (float)g_api->get_float(g_id, "cam_dist", 6.0);
        float h    = (float)g_api->get_float(g_id, "cam_height", 3.0);
        float yaw  = (float)g_api->get_float(g_id, "cam_yaw", 0.0) * 0.017453292f;
        Vec3 target = { p.x + sinf(yaw) * dist, p.y + h, p.z + cosf(yaw) * dist };
        if (!g_cam_pos_valid) { g_cam_pos = target; g_cam_pos_valid = 1; }
        float k = 1.f - expf((float)(-lerp) * dt);
        g_cam_pos.x += (target.x - g_cam_pos.x) * k;
        g_cam_pos.y += (target.y - g_cam_pos.y) * k;
        g_cam_pos.z += (target.z - g_cam_pos.z) * k;
        float sh = (float)g_api->get_float(g_id, "cam_shake", 0.0);
        Vec3 pos = g_cam_pos;
        if (sh > 0.f) {
            pos.x += sh * 0.05f * sinf(g_time_t * 43.f);
            pos.y += sh * 0.05f * sinf(g_time_t * 57.f + 1.f);
        }
        fn_tr_set_position(cam_tr, pos, mi_tr_set_position);
        fn_tr_set_rotation(cam_tr, quat_lookat(pos, p), mi_tr_set_rotation);
    } else {           /* плавное следование */
        double lerp = g_api->get_float(g_id, "cam_lerp", 6.0);
        if (lerp < 0.1) lerp = 0.1;
        Vec3 target = { p.x, p.y + 3.5f, p.z - 6.f };
        if (!g_cam_pos_valid) { g_cam_pos = target; g_cam_pos_valid = 1; }
        float k = 1.f - expf((float)(-lerp) * dt);
        g_cam_pos.x += (target.x - g_cam_pos.x) * k;
        g_cam_pos.y += (target.y - g_cam_pos.y) * k;
        g_cam_pos.z += (target.z - g_cam_pos.z) * k;
        float sh = (float)g_api->get_float(g_id, "cam_shake", 0.0);
        if (sh > 0.f) {
            g_cam_pos.x += sh * 0.05f * sinf(g_time_t * 43.f);
            g_cam_pos.y += sh * 0.05f * sinf(g_time_t * 57.f + 1.f);
        }
        fn_tr_set_position(cam_tr, g_cam_pos, mi_tr_set_position);
        fn_tr_set_rotation(cam_tr, quat_lookat(g_cam_pos, p), mi_tr_set_rotation);
    }
}

/* ================= ЦВЕТНОЙ СВЕТ (v1.6) =================
   Красим все Light'ы сцены через Object.FindObjectsOfTypeAll(typeof(Light)).
   Оригинальные цвета запоминаются и восстанавливаются при выключении. */
static Color preset_sky_color(void) {
    const char* name = g_api->get_string(g_id, "sky_preset", "off");
    float k = (float)g_api->get_float(g_id, "sky_intensity", 1.0);
    if (k < 0.f) k = 0.f;
    for (int i = 1; i < kSkyPresetCount; i++)
        if (!strcmp(name, kSkyPresets[i].name))
            return (Color){ kSkyPresets[i].sky.r*k, kSkyPresets[i].sky.g*k, kSkyPresets[i].sky.b*k, 1.f };
    return hsv2rgb(g_disco_hue);   /* disco/pulse/beauty без пресета */
}

static Color light_mode_color(const char* mode, int i) {
    if (!strcmp(mode, "_preset")) return preset_sky_color();
    if (!strcmp(mode, "fire"))  return (Color){1.f, 0.45f, 0.1f, 1.f};
    if (!strcmp(mode, "ice"))   return (Color){0.4f, 0.75f, 1.f, 1.f};
    if (!strcmp(mode, "rose"))  return (Color){1.f, 0.35f, 0.6f, 1.f};
    if (!strcmp(mode, "rainbow")) return hsv2rgb(g_hue + (float)i * 0.07f);
    return (Color){1.f, 1.f, 1.f, 1.f};
}

static void lights_restore(void) {
    if (!fn_light_color) return;
    for (int i = 0; i < 24; i++) {
        if (!g_light_orig[i].ptr) continue;
        if (!obj_alive(g_light_orig[i].ptr)) { g_light_orig[i].ptr = 0; continue; }
        fn_light_color(g_light_orig[i].ptr, g_light_orig[i].orig, mi_light_color);
        g_light_orig[i].ptr = 0;
    }
    g_lights_tinted = 0;
}

static void light_tick(void) {
    const char* mode = g_api->get_string(g_id, "light_tint", "off");
    int beauty = g_api->get_bool(g_id, "beauty_mode", 0);
    int on = strcmp(mode, "off") != 0 || beauty;
    if (beauty && !strcmp(mode, "off")) mode = "rainbow";
    /* v1.7: пресет неба/диско автоматически красят и свет сцены,
       иначе пресет заметен только на объектах с ambient ("таблички") */
    if (!on) {
        int sky_active = g_api->get_bool(g_id, "disco_sky", 0)
                      || g_api->get_bool(g_id, "sky_pulse", 0)
                      || g_api->get_bool(g_id, "beauty_mode", 0)
                      || strcmp(g_api->get_string(g_id, "sky_preset", "off"), "off") != 0;
        if (sky_active) { mode = "_preset"; on = 1; }
    }

    /* туман - отдельный тумблер */
    int fog_want = g_api->get_bool(g_id, "fog_on", 0);
    if (fn_set_fog && fog_want != g_fog_on) {
        fn_set_fog(fog_want, mi_set_fog);
        g_fog_on = fog_want;
    }

    if (!on) {
        if (g_lights_tinted) lights_restore();
        return;
    }
    if (!fn_find_objects || !fn_light_color || !g_light_type) return;

    /* переполучаем массив раз в 5 секунд (на сцене набор света меняется) */
    if (!g_lights_arr || g_time_t - g_lights_fetched > 5.f) {
        g_lights_arr = fn_find_objects(g_light_type, mi_find_objects);
        g_lights_fetched = g_time_t;
    }
    if (!g_lights_arr) return;

    int n = *(int*)((char*)g_lights_arr + 0x18);   /* Il2CppArray::max_length */
    if (n > 24) n = 24;
    void** items = (void**)((char*)g_lights_arr + 0x20);
    for (int i = 0; i < n; i++) {
        void* l = items[i];
        if (!l || !obj_alive(l)) continue;
        if (fn_light_getcolor) {
            int known = 0;
            for (int j = 0; j < 24; j++)
                if (g_light_orig[j].ptr == l) { known = 1; break; }
            if (!known) {
                for (int j = 0; j < 24; j++)
                    if (!g_light_orig[j].ptr) {
                        g_light_orig[j].ptr = l;
                        g_light_orig[j].orig = fn_light_getcolor(l, mi_light_getcolor);
                        break;
                    }
            }
        }
        fn_light_color(l, light_mode_color(mode, i), mi_light_color);
    }
    if (n > 0) {
        static int g_lights_logged;
        if (!g_lights_logged) {
            g_lights_logged = 1;
            char b[96];
            snprintf(b, sizeof b, "FishVisuals: light tint applied to %d lights", n);
            g_api->log(g_id, b);
        }
    }
    g_lights_tinted = 1;
}

static void grade_tick(void) {
    const char* mode = g_api->get_string(g_id, "color_grade", "off");
    int idx = 0;
    if (!strcmp(mode, "flat"))  idx = 1;
    if (!strcmp(mode, "unlit")) idx = 2;
    if (!strcmp(mode, "solid")) idx = 3;
    if (idx == g_grade_applied) return;
    if (!fn_cam_main) return;
    /* ленивый резолв шейдеров (в init не звоним - ранний Shader.Find может крашить) */
    if (idx > 0 && !g_sh_flat && fn_shader_find) {
        void* n1 = bearite_il2.string_new("Sprites/Default");
        void* n2 = bearite_il2.string_new("Unlit/Texture");
        g_sh_flat  = n1 ? fn_shader_find(n1, mi_shader_find) : 0;
        g_sh_unlit = n2 ? fn_shader_find(n2, mi_shader_find) : 0;
        g_api->log(g_id, (g_sh_flat && g_sh_unlit) ? "FishVisuals: grade shaders OK"
                                                   : "FishVisuals: grade shaders MISS");
    }
    void* cam = fn_cam_main(mi_cam_main);
    if (!cam) return;

    if (g_grade_applied > 0) {   /* сначала откатываем старое */
        if (fn_cam_resetshader) fn_cam_resetshader(cam, mi_cam_resetshader);
        if (fn_cam_set_clearflags) fn_cam_set_clearflags(cam, 1, mi_set_clearflags); /* Skybox */
    }
    if (idx == 1 && g_sh_flat && fn_cam_replshader) {
        void* tag = bearite_il2.string_new("");
        if (tag) fn_cam_replshader(cam, g_sh_flat, tag, mi_cam_replshader);
    } else if (idx == 2 && g_sh_unlit && fn_cam_replshader) {
        void* tag = bearite_il2.string_new("");
        if (tag) fn_cam_replshader(cam, g_sh_unlit, tag, mi_cam_replshader);
    } else if (idx == 3) {
        if (fn_cam_set_clearflags) fn_cam_set_clearflags(cam, 2, mi_set_clearflags); /* SolidColor */
        if (fn_cam_set_bgcolor) fn_cam_set_bgcolor(cam, (Color){0.02f, 0.02f, 0.06f, 1.f}, mi_set_bgcolor);
    }
    g_grade_applied = idx;
    g_api->log(g_id, "FishVisuals: color grade applied");
}

/* ================= ОРБИТАЛЬНЫЕ СПУТНИКИ (v1.5) =================
   "Партиклы вокруг игрока" без готовых префабов: клонируем TrailRenderer
   игрока (Object.Instantiate), отсоединяем от родителя и крутим вокруг
   игрока на орбитах. Каждый спутник - цветная комета (цвет по режиму). */
static Color orbit_color(const char* mode, int i, float t) {
    if (!strcmp(mode, "bees"))   return ((int)(t * 8.f) + i) % 2 ? (Color){1.f, 0.85f, 0.1f, 1.f}
                                                                : (Color){0.15f, 0.12f, 0.05f, 1.f};
    if (!strcmp(mode, "hearts")) return (Color){1.f, 0.25f, 0.55f, 1.f};
    if (!strcmp(mode, "stars"))  return (Color){1.f, 0.95f, 0.5f, 1.f};
    if (!strcmp(mode, "rainbow")) return hsv2rgb(g_hue + (float)i * 0.13f);
    return (Color){0.45f, 0.85f, 1.f, 1.f};   /* bubbles */
}

static void orbit_tick(float dt) {
    const char* mode = g_api->get_string(g_id, "orbit_fx", "off");
    int beauty = g_api->get_bool(g_id, "beauty_mode", 0);
    int need = (int)g_api->get_float(g_id, "orbit_count", 4.0);
    if (need < 1) need = 1;
    if (need > 8) need = 8;
    if (beauty) { if (!strcmp(mode, "off")) mode = "rainbow"; }
    int on = strcmp(mode, "off") != 0;

    if (!on) {
        if (g_orbit_on > 0 && fn_go_setactive) {
            for (int i = 0; i < g_orbit_created; i++)
                if (g_orbit_go[i]) fn_go_setactive(g_orbit_go[i], 0, mi_go_setactive);
            g_orbit_on = 0;
        }
        return;
    }
    /* v1.6.5: НЕ клонируем трейл игрока (Instantiate(TrailRenderer) копирует
       весь GameObject игрока вместе с PlayerBearController = второй "локальный"
       медведь). Создаём чистый GameObject через il2cpp_object_new + .ctor. */
    if (!fn_obj_new || !fn_go_ctor || !g_go_klass || !fn_go_addcomponent ||
        !fn_trail_time || !g_trail_type) {
        if (!g_cine_warned) { g_cine_warned = 1;
            g_api->log(g_id, "FishVisuals: ORBIT невозможен - нет il2cpp_object_new, фича отключена"); }
        return;
    }

    while (g_orbit_created < need) {
        void* go = fn_obj_new(g_go_klass);
        if (!go) return;
        void* nm = bearite_il2.string_new("fish_orbit");
        fn_go_ctor(go, nm, mi_go_ctor);
        void* new_tr = fn_go_addcomponent(go, g_trail_type, mi_go_addcomponent);
        if (!new_tr) return;
        if (fn_trail_time)       fn_trail_time(new_tr, 0.3f, mi_trail_time);
        if (fn_trail_startwidth) fn_trail_startwidth(new_tr, 0.14f, mi_trail_startwidth);
        if (fn_trail_endwidth)   fn_trail_endwidth(new_tr, 0.01f, mi_trail_endwidth);
        if (fn_trail_mvd)        fn_trail_mvd(new_tr, 0.01f, mi_trail_mvd);
        /* v1.7.1: без материала трейл рисуется маджентой - берём материал
           трейла игрока (там уже частичный шейдер) */
        if (g_trail && fn_trail_getmaterial && fn_trail_setmaterial) {
            void* mat = fn_trail_getmaterial(g_trail, mi_trail_getmaterial);
            if (mat) fn_trail_setmaterial(new_tr, mat, mi_trail_setmaterial);
        }
        g_hnd_orbit[g_orbit_created] = gc_pin(go);
        g_orbit_tr[g_orbit_created] = new_tr;
        g_orbit_go[g_orbit_created] = go;
        g_orbit_created++;
    }

    float radius = (float)g_api->get_float(g_id, "orbit_radius", 1.5);
    g_orbit_on = need;
    for (int i = 0; i < g_orbit_created; i++) {
        int active = i < need;
        if (fn_go_setactive) fn_go_setactive(g_orbit_go[i], active ? 1 : 0, mi_go_setactive);
        if (!active || !g_orbit_tr[i]) continue;
        if (!obj_alive(g_orbit_tr[i])) { g_orbit_tr[i] = 0; g_orbit_go[i] = 0; continue; }
        Color c = orbit_color(mode, i, g_time_t);
        Color e = c; e.a = 0.f;
        if (fn_trail_startcolor) fn_trail_startcolor(g_orbit_tr[i], c, mi_trail_startcolor);
        if (fn_trail_endcolor)   fn_trail_endcolor(g_orbit_tr[i], e, mi_trail_endcolor);
        if (!g_last_pos_valid) continue;
        float ph = g_time_t * 2.2f + (float)i * 6.2831853f / (float)need;
        Vec3 p = g_last_pos;
        p.x += radius * cosf(ph);
        p.z += radius * sinf(ph);
        p.y += 0.9f + 0.25f * sinf(g_time_t * 3.f + (float)i);
        void* tr = fn_get_transform(g_orbit_tr[i], mi_get_transform);
        if (tr && fn_tr_set_position) fn_tr_set_position(tr, p, mi_tr_set_position);
    }
}

/* ================= ТЕКСТ БОССОВ / ШИКИ (v1.9) ================= */
static Color boss_name_color(const char* name) {
    if (!strcmp(name, "red"))    return (Color){1.f, 0.15f, 0.15f, 1.f};
    if (!strcmp(name, "orange")) return (Color){1.f, 0.55f, 0.1f, 1.f};
    if (!strcmp(name, "gold"))   return (Color){1.f, 0.85f, 0.2f, 1.f};
    if (!strcmp(name, "green"))  return (Color){0.3f, 1.f, 0.4f, 1.f};
    if (!strcmp(name, "cyan"))   return (Color){0.2f, 0.95f, 1.f, 1.f};
    if (!strcmp(name, "blue"))   return (Color){0.25f, 0.5f, 1.f, 1.f};
    if (!strcmp(name, "purple")) return (Color){0.7f, 0.35f, 1.f, 1.f};
    if (!strcmp(name, "pink"))   return (Color){1.f, 0.35f, 0.75f, 1.f};
    if (!strcmp(name, "white"))  return (Color){1.f, 1.f, 1.f, 1.f};
    if (!strcmp(name, "black"))  return (Color){0.02f, 0.02f, 0.02f, 1.f};
    return (Color){0, 0, 0, 0};   /* off */
}

static void (*orig_show_boss)(void*, void*, const void*);
static int g_dbg_boss;

static void hk_show_boss(void* self, void* key, const void* mi) {
    orig_show_boss(self, key, mi);
    void* label = self ? *(void**)((char*)self + 0x120) : 0;   /* GameManager.bossNameLabel */
    if (!label || !fn_tmp_settext) return;
    /* v1.10.1: string-настройка - пустое поле = выкл, иначе свой текст */
    const char* txt = g_api->get_string(g_id, "boss_text", "");
    if (txt && txt[0]) {
        void* str = bearite_il2.string_new(txt);
        if (str) fn_tmp_settext(label, str, mi_tmp_settext);
        if (!g_dbg_boss) { g_dbg_boss = 1; g_api->log(g_id, "FishVisuals: BOSS TEXT применён"); }
    }
    const char* col = g_api->get_string(g_id, "boss_name_color", "off");
    if (strcmp(col, "off") != 0 && fn_tmp_setcolor) {
        Color c = boss_name_color(col);
        if (c.a > 0.f) fn_tmp_setcolor(label, c, mi_tmp_setcolor);
    }
}

/* Палитра цветов для босса/Шики (общая) */
static void* tag_color_rgb(const char* name, float* rainbow) {
    *rainbow = 0;
    if (!strcmp(name, "red"))    return (void*)(Color[]){(Color){1.f, 0.15f, 0.15f, 1.f}};
    if (!strcmp(name, "orange")) return (void*)(Color[]){(Color){1.f, 0.55f, 0.1f, 1.f}};
    if (!strcmp(name, "gold"))   return (void*)(Color[]){(Color){1.f, 0.85f, 0.2f, 1.f}};
    if (!strcmp(name, "green"))  return (void*)(Color[]){(Color){0.3f, 1.f, 0.4f, 1.f}};
    if (!strcmp(name, "cyan"))   return (void*)(Color[]){(Color){0.2f, 0.95f, 1.f, 1.f}};
    if (!strcmp(name, "blue"))   return (void*)(Color[]){(Color){0.25f, 0.5f, 1.f, 1.f}};
    if (!strcmp(name, "purple")) return (void*)(Color[]){(Color){0.7f, 0.35f, 1.f, 1.f}};
    if (!strcmp(name, "pink"))   return (void*)(Color[]){(Color){1.f, 0.35f, 0.75f, 1.f}};
    if (!strcmp(name, "rainbow")) { *rainbow = 1; return 0; }
    return (void*)(Color[]){(Color){1.f, 1.f, 1.f, 1.f}};   /* white */
}

static void (*orig_shicka_update)(void*, const void*);
static int g_dbg_shicka;

static void hk_shicka_update(void* self, const void* mi) {
    orig_shicka_update(self, mi);
    if (!self || !fn_tmp_settext) return;
    const char* txt = g_api->get_string(g_id, "shicka_text", "");
    if (!txt || !txt[0]) return;
    void* label = *(void**)((char*)self + 0x30);   /* ShickaTipsUI.textDisplay */
    if (!label) return;
    void* str = bearite_il2.string_new(txt);
    if (str) fn_tmp_settext(label, str, mi_tmp_settext);
    /* v1.11.4: цвет подсказок Шики (по новому запросу) */
    if (fn_tmp_setcolor) {
        float rb = 0;
        void* rgbp = tag_color_rgb(g_api->get_string(g_id, "shicka_text_color", "white"), &rb);
        Color cc = rb ? hsv2rgb(g_hue) : *(Color*)rgbp;
        fn_tmp_setcolor(label, cc, mi_tmp_setcolor);
    }
    if (!g_dbg_shicka) { g_dbg_shicka = 1; g_api->log(g_id, "FishVisuals: SHICKA TEXT применён"); }
}

/* v1.10.6: процедурная мягкая круглая текстура 64x64 (радиальный градиент).
   Гарантированно круглые частицы в любом режиме, не зависит от ассетов игры. */
static const unsigned char kSoftPng[] = {
137, 80, 78, 71, 13, 10, 26, 10, 0, 0, 0, 13, 73, 72, 68, 82, 0, 0, 0, 64, 0, 0, 0, 64, 8, 6, 0, 0, 0, 170, 105, 113, 222, 0, 0, 6, 56, 73, 68, 65, 84, 120, 218, 237, 155, 205, 75, 213, 89, 24, 199, 189, 141, 169, 57, 142, 102, 142, 222, 76, 237, 230, 59, 166, 230, 245, 173, 188, 147, 136, 74, 56, 25, 134, 168, 8, 98, 49, 40, 169, 5, 201, 140, 46, 68, 193, 176, 197, 80, 130, 27, 195, 8, 98, 22, 33, 179, 112, 35, 109, 6, 34, 104, 209, 78, 104, 219, 170, 85, 219, 254, 129, 249, 3, 156, 231, 7, 159, 7, 30, 14, 191, 59, 247, 250, 50, 83, 51, 157, 197, 103, 225, 185, 231, 60, 231, 124, 159, 183, 243, 187, 215, 123, 51, 246, 247, 247, 51, 190, 102, 50, 188, 3, 62, 239, 1, 34, 240, 191, 118, 64, 36, 132, 19, 73, 8, 155, 251, 159, 116, 64, 152, 216, 111, 28, 50, 29, 220, 215, 195, 156, 242, 197, 59, 32, 76, 180, 10, 60, 105, 200, 74, 130, 157, 99, 29, 227, 58, 227, 139, 115, 128, 21, 110, 35, 108, 197, 102, 11, 57, 134, 83, 14, 246, 181, 108, 199, 41, 153, 78, 102, 28, 155, 35, 142, 75, 252, 137, 16, 209, 217, 70, 116, 32, 48, 87, 248, 22, 242, 146, 160, 175, 231, 26, 167, 100, 27, 135, 156, 116, 178, 34, 242, 57, 29, 16, 9, 17, 159, 149, 68, 244, 119, 144, 47, 20, 192, 105, 7, 29, 207, 55, 243, 195, 156, 145, 21, 226, 132, 200, 191, 237, 128, 176, 168, 91, 225, 26, 229, 124, 35, 182, 80, 56, 35, 20, 9, 223, 39, 161, 136, 57, 133, 198, 41, 249, 38, 59, 172, 35, 142, 37, 27, 142, 67, 188, 70, 253, 148, 137, 118, 62, 2, 172, 224, 18, 33, 42, 156, 133, 82, 7, 29, 143, 50, 215, 58, 228, 180, 201, 12, 117, 68, 178, 108, 248, 71, 29, 96, 197, 107, 173, 231, 144, 166, 121, 142, 112, 21, 173, 98, 203, 132, 114, 161, 66, 56, 15, 49, 208, 191, 43, 152, 83, 102, 156, 162, 206, 176, 142, 200, 99, 207, 28, 211, 27, 14, 229, 132, 163, 136, 119, 163, 94, 64, 234, 90, 225, 231, 16, 20, 136, 187, 32, 84, 10, 213, 66, 77, 18, 170, 153, 115, 129, 53, 229, 216, 176, 142, 40, 100, 47, 55, 27, 14, 229, 132, 227, 18, 175, 81, 47, 54, 194, 43, 136, 110, 21, 226, 234, 132, 122, 161, 65, 184, 40, 52, 58, 92, 228, 181, 122, 230, 214, 176, 54, 134, 45, 117, 68, 177, 201, 134, 35, 59, 225, 168, 226, 53, 229, 139, 136, 80, 41, 81, 83, 225, 181, 8, 10, 196, 53, 9, 151, 132, 22, 161, 21, 218, 64, 255, 110, 97, 78, 19, 107, 234, 177, 161, 142, 40, 103, 143, 18, 246, 212, 146, 56, 180, 19, 210, 117, 128, 109, 120, 86, 188, 166, 124, 212, 68, 189, 146, 232, 213, 19, 217, 102, 33, 46, 180, 11, 157, 194, 21, 161, 75, 72, 56, 116, 241, 90, 39, 115, 227, 172, 109, 196, 86, 13, 182, 53, 27, 162, 166, 36, 172, 19, 108, 99, 60, 178, 3, 52, 250, 153, 166, 225, 217, 180, 87, 241, 101, 212, 108, 21, 233, 219, 192, 225, 131, 168, 118, 32, 236, 7, 161, 91, 232, 17, 122, 133, 62, 135, 94, 94, 235, 102, 238, 21, 214, 182, 98, 171, 1, 219, 85, 236, 85, 102, 156, 96, 203, 33, 199, 56, 33, 101, 22, 28, 52, 245, 115, 141, 248, 34, 35, 62, 70, 3, 211, 168, 183, 16, 197, 64, 196, 85, 132, 5, 34, 175, 9, 3, 194, 117, 97, 80, 184, 1, 131, 140, 13, 48, 167, 143, 53, 87, 177, 209, 142, 77, 205, 134, 106, 246, 84, 39, 20, 25, 39, 228, 30, 164, 20, 210, 77, 125, 173, 251, 60, 58, 240, 25, 234, 240, 28, 209, 80, 241, 77, 164, 110, 39, 105, 29, 136, 232, 71, 88, 32, 114, 72, 24, 22, 70, 132, 81, 97, 12, 70, 25, 27, 102, 206, 32, 107, 250, 177, 145, 192, 102, 156, 61, 212, 9, 231, 57, 67, 9, 103, 42, 224, 140, 218, 15, 82, 150, 194, 65, 83, 95, 235, 190, 152, 102, 84, 65, 74, 170, 248, 32, 93, 47, 19, 185, 94, 162, 25, 136, 185, 137, 192, 113, 97, 66, 152, 20, 110, 9, 183, 225, 22, 99, 19, 204, 25, 97, 205, 32, 54, 122, 177, 121, 153, 61, 212, 9, 85, 156, 161, 148, 51, 217, 126, 144, 86, 41, 164, 138, 190, 237, 250, 182, 238, 207, 210, 145, 43, 169, 203, 70, 162, 163, 226, 251, 136, 224, 144, 17, 30, 8, 252, 73, 152, 22, 102, 132, 89, 97, 14, 102, 25, 155, 102, 206, 164, 113, 196, 16, 182, 250, 140, 19, 226, 236, 89, 199, 25, 202, 57, 147, 237, 7, 238, 173, 144, 182, 3, 108, 237, 187, 209, 183, 169, 31, 163, 51, 55, 80, 159, 157, 70, 252, 143, 68, 112, 140, 168, 6, 162, 238, 8, 119, 133, 251, 194, 207, 194, 130, 176, 8, 11, 140, 221, 103, 206, 29, 214, 76, 96, 227, 38, 54, 213, 9, 157, 236, 217, 192, 25, 98, 78, 41, 184, 89, 144, 180, 23, 164, 74, 127, 91, 251, 54, 250, 54, 245, 155, 105, 82, 9, 82, 117, 192, 136, 15, 34, 57, 69, 148, 231, 17, 187, 36, 172, 8, 171, 194, 3, 88, 101, 108, 137, 57, 243, 172, 153, 194, 134, 58, 97, 128, 61, 18, 236, 217, 236, 148, 130, 205, 2, 183, 23, 28, 200, 1, 110, 231, 15, 139, 126, 45, 105, 216, 74, 167, 238, 161, 94, 135, 140, 248, 32, 165, 239, 9, 191, 32, 46, 16, 250, 80, 248, 85, 120, 44, 172, 195, 99, 198, 30, 50, 103, 137, 53, 247, 176, 161, 78, 24, 98, 143, 30, 246, 108, 229, 12, 181, 73, 178, 32, 55, 85, 25, 164, 219, 252, 10, 184, 106, 180, 246, 109, 244, 59, 72, 203, 126, 154, 214, 8, 169, 59, 133, 128, 32, 189, 151, 133, 53, 225, 145, 176, 33, 108, 10, 91, 194, 83, 216, 98, 108, 131, 57, 107, 172, 89, 192, 198, 20, 54, 71, 216, 163, 159, 61, 59, 156, 44, 208, 94, 80, 196, 153, 83, 54, 195, 84, 14, 112, 155, 95, 41, 87, 79, 13, 143, 170, 113, 19, 125, 77, 253, 113, 234, 119, 142, 40, 46, 19, 217, 117, 68, 62, 19, 126, 19, 94, 8, 219, 240, 130, 177, 103, 204, 89, 103, 205, 50, 54, 230, 176, 57, 110, 74, 65, 179, 32, 206, 89, 106, 56, 91, 105, 72, 51, 60, 144, 3, 194, 30, 124, 10, 73, 173, 50, 222, 169, 213, 113, 21, 181, 243, 212, 214, 103, 162, 63, 73, 19, 155, 39, 149, 215, 16, 244, 68, 120, 142, 224, 29, 97, 87, 120, 9, 187, 140, 109, 51, 231, 9, 107, 214, 176, 49, 143, 205, 73, 147, 5, 125, 236, 221, 206, 89, 234, 56, 91, 25, 103, 45, 76, 242, 96, 148, 182, 3, 220, 238, 31, 53, 87, 95, 61, 111, 90, 58, 121, 116, 213, 218, 215, 232, 223, 165, 153, 173, 146, 210, 155, 8, 251, 29, 177, 127, 8, 175, 133, 55, 240, 154, 177, 93, 230, 60, 103, 205, 35, 108, 44, 98, 83, 179, 64, 123, 65, 55, 103, 184, 196, 153, 244, 74, 140, 134, 220, 6, 135, 118, 128, 173, 255, 10, 158, 192, 244, 234, 179, 233, 63, 76, 157, 78, 115, 157, 45, 145, 198, 27, 164, 246, 54, 2, 95, 9, 111, 133, 61, 225, 29, 236, 49, 246, 138, 57, 219, 172, 217, 192, 198, 18, 54, 167, 217, 99, 216, 41, 3, 189, 18, 171, 205, 109, 224, 246, 129, 180, 28, 224, 222, 255, 250, 232, 171, 215, 159, 173, 255, 86, 222, 193, 245, 242, 28, 175, 233, 63, 195, 157, 190, 66, 103, 223, 164, 190, 119, 136, 242, 91, 68, 191, 23, 62, 192, 123, 198, 222, 50, 103, 135, 53, 155, 216, 88, 193, 230, 140, 41, 131, 235, 236, 221, 197, 89, 108, 31, 208, 235, 80, 31, 141, 221, 231, 129, 191, 117, 128, 109, 128, 121, 73, 26, 160, 94, 127, 9, 83, 255, 163, 60, 210, 206, 210, 189, 87, 185, 222, 182, 104, 114, 187, 164, 250, 30, 130, 63, 10, 159, 224, 35, 99, 123, 204, 217, 101, 205, 22, 54, 86, 177, 57, 203, 30, 163, 166, 15, 36, 204, 117, 24, 214, 8, 243, 66, 26, 97, 168, 3, 34, 105, 56, 32, 102, 28, 208, 102, 28, 112, 131, 123, 250, 54, 29, 123, 145, 7, 156, 117, 174, 185, 109, 154, 221, 27, 34, 253, 1, 225, 127, 194, 39, 198, 222, 49, 231, 37, 107, 158, 98, 227, 1, 54, 231, 216, 99, 140, 61, 213, 1, 109, 198, 1, 177, 52, 28, 16, 241, 14, 240, 37, 224, 155, 160, 191, 6, 253, 131, 144, 127, 20, 246, 111, 134, 252, 219, 97, 255, 129, 136, 255, 72, 204, 127, 40, 234, 63, 22, 247, 255, 24, 241, 255, 26, 243, 255, 28, 245, 255, 30, 247, 95, 144, 240, 95, 145, 241, 95, 146, 242, 95, 147, 243, 95, 148, 244, 95, 149, 245, 95, 150, 246, 95, 151, 247, 63, 152, 240, 63, 153, 241, 63, 154, 242, 63, 155, 243, 63, 156, 244, 63, 157, 245, 63, 158, 254, 122, 248, 11, 110, 209, 210, 196, 216, 222, 19, 141, 0, 0, 0, 0, 73, 69, 78, 68, 174, 66, 96, 130
};

/* v1.11.9: мягкая круглая текстура через встроенный PNG + ImageConversion.LoadImage.
   Путь SetPixels32/Apply на этом устройстве падает/виснет - LoadImage надёжен. */
static void* make_soft_texture(void) {
    if (!fn_obj_new || !il_array_new || !il_array_class_get ||
        !fn_tex_ctor || !fn_tex_loadimage || !g_byte_klass) return 0;
    void* tex_klass = bearite_find_class("UnityEngine", "Texture2D");
    if (!tex_klass) return 0;
    void* tex = fn_obj_new(tex_klass);
    if (!tex) return 0;
    fn_tex_ctor(tex, 4, 4, 4 /*TextureFormat.RGBA32*/, 0, mi_tex_ctor);
    void* arr_klass = il_array_class_get(g_byte_klass, 1);
    if (!arr_klass) { g_api->log(g_id, "FishVisuals: TEX arr_klass FAIL"); return 0; }
    void* arr = il_array_new(arr_klass, (int)sizeof kSoftPng);
    if (!arr) { g_api->log(g_id, "FishVisuals: TEX array_new FAIL"); return 0; }
    memcpy((char*)arr + 0x20, kSoftPng, sizeof kSoftPng);
    int ok = fn_tex_loadimage(tex, arr, mi_tex_loadimage);
    if (!ok) { g_api->log(g_id, "FishVisuals: TEX LoadImage вернул false"); return 0; }
    g_api->log(g_id, "FishVisuals: TEX LoadImage ок - текстура готова");
    gc_pin(tex);
    gc_pin(arr);
    return tex;
}

/* v1.10.3: дефолтный материал частиц в этом билде битый (magenta),
   поэтому ищем живой ParticleSystem в сцене, клонируем его материал
   (мягкая текстура) и отбеливаем - наши Emit-цвета будут видны как надо. */
static void* find_soft_particle_material(void) {
    if (g_soft_mat || g_soft_tried) return g_soft_mat;
    g_soft_tried = 1;

    /* v1.10.8: база - материал ТРЕЙЛА (доказанно рендерит цветное в этом билде),
       а не украденный у сцены (у него кастомный освещаемый шейдер - отсюда
       "белые квадраты, сереют при повороте камеры"). */
    void* tex = make_soft_texture();
    if (!tex) g_api->log(g_id, "FishVisuals: ВНИМАНИЕ - процедурная текстура не создана, частицы будут квадратными");

    void* base = 0;
    if (g_trail && fn_trail_getmaterial)
        base = fn_trail_getmaterial(g_trail, mi_trail_getmaterial);
    /* запас: украсть у сцены, если трейла нет */
    if (!base && fn_find_objects && g_ps_type && g_psrend_type) {
        void* arr = fn_find_objects(g_ps_type, mi_find_objects);
        if (arr) {
            int n = *(int*)((char*)arr + 0x18);
            if (n > 24) n = 24;
            void** items = (void**)((char*)arr + 0x20);
            for (int i = 0; i < n && !base; i++) {
                void* go = items[i] && fn_get_gameobject ? fn_get_gameobject(items[i], mi_get_gameobject) : 0;
                void* rend = go ? fn_go_getcomponent(go, g_psrend_type, mi_go_getcomponent) : 0;
                if (rend) base = fn_trail_getmaterial(rend, mi_trail_getmaterial);
            }
        }
    }
    if (!base) { g_api->log(g_id, "FishVisuals: soft material - нет базового материала"); return 0; }

    void* clone = fn_object_instantiate(base, mi_object_instantiate);
    if (!clone) { g_soft_mat = base; g_api->log(g_id, "FishVisuals: soft material = оригинал (клон не создался)"); return g_soft_mat; }
    /* шейдер Particles/Standard Unlit (он же у трейла - работает) */
    if (!g_sh_particles && fn_shader_find) {
        void* shn = bearite_il2.string_new("Particles/Standard Unlit");
        g_sh_particles = shn ? fn_shader_find(shn, mi_shader_find) : 0;
    }
    if (g_sh_particles && fn_mat_setshader)
        fn_mat_setshader(clone, g_sh_particles, mi_mat_setshader);
    if (tex && fn_mat_setmaintex)
        fn_mat_setmaintex(clone, tex, mi_mat_setmaintex);
    else if (!tex && fn_mat_getmaintex && fn_mat_setmaintex)
        fn_mat_setmaintex(clone, fn_mat_getmaintex(base, mi_mat_getmaintex), mi_mat_setmaintex);
    if (fn_set_color)
        fn_set_color(clone, (Color){1.f, 1.f, 1.f, 1.f}, mi_set_color);
    gc_pin(clone);
    if (tex) gc_pin(tex);
    g_soft_mat = clone;

    /* верификация: читаем назад шейдер и текстуру */
    void* rt = fn_mat_getmaintex ? fn_mat_getmaintex(clone, mi_mat_getmaintex) : 0;
    char b[160];
    snprintf(b, sizeof b, "FishVisuals: soft material готов: shader=%p tex=%s",
             g_sh_particles, rt ? "OK" : "NULL!");
    g_api->log(g_id, b);
    return g_soft_mat;
}

static void apply_soft_material(void* go) {
    if (!go || !g_soft_mat || !fn_go_getcomponent || !fn_trail_setmaterial) return;
    void* rend = fn_go_getcomponent(go, g_psrend_type, mi_go_getcomponent);
    if (rend) fn_trail_setmaterial(rend, g_soft_mat, mi_trail_setmaterial);
}

/* ================= ФЕЙКОВОЕ СОЛНЦЕ (v1.8) =================
   Блики/лупа как таковые требуют .flare-ассет (недоступен из кода),
   поэтому делаем светящееся пятно: ParticleSystem с выключенной эмиссией,
   вручную спавним крупную мягкую частицу в направлении "солнца". */
static void sun_tick(void) {
    int on = g_api->get_bool(g_id, "sun_glare", 0);
    if (!on) {
        if (g_sun_go && fn_go_setactive) { fn_go_setactive(g_sun_go, 0, mi_go_setactive); g_sun_ps = 0; }
        return;
    }
    if (g_sun_ps && !obj_alive(g_sun_ps)) {   /* умер при смене сцены - пересоздадим */
        g_sun_ps = 0; g_sun_go = 0;
    }
    if (!g_sun_ps) {
        if (!fn_obj_new || !g_go_klass || !fn_go_addcomponent || !g_ps_type || !fn_go_ctor) return;
        void* go = fn_obj_new(g_go_klass);
        void* nm = bearite_il2.string_new("fish_sun");
        fn_go_ctor(go, nm, mi_go_ctor);
        g_sun_ps = fn_go_addcomponent(go, g_ps_type, mi_go_addcomponent);
        g_sun_go = go;
        gc_pin(go);   /* v1.10.2: защита от GC */
        if (!g_sun_ps) return;
        find_soft_particle_material();
        apply_soft_material(go);
        if (fn_ps_getemission && fn_em_set_enabled) {
            void* em_val = fn_ps_getemission(g_sun_ps, mi_ps_getemission);
            void* em_ptr = &em_val;   /* struct byref: указатель на значение из x0 */
            fn_em_set_enabled(em_ptr, 0, mi_em_enabled);
        }
        g_api->log(g_id, "FishVisuals: SUN создано");
    }
    if (!g_last_pos_valid || !fn_ps_emit5) return;

    float yaw = (float)g_api->get_float(g_id, "sun_yaw", 45.0) * 0.017453292f;
    float pit = (float)g_api->get_float(g_id, "sun_height", 45.0) * 0.017453292f;
    const float R = 500.f;
    Vec3 pos = {
        g_last_pos.x + R * cosf(pit) * sinf(yaw),
        g_last_pos.y + R * sinf(pit),
        g_last_pos.z + R * cosf(pit) * cosf(yaw) };

    if (g_time_t >= g_sun_next_emit) {
        g_sun_next_emit = g_time_t + 0.5f;
        float size = (float)g_api->get_float(g_id, "sun_size", 60.0);
        const char* col = g_api->get_string(g_id, "sun_color", "warm");
        Color32 c = {255, 235, 190, 220};
        if (!strcmp(col, "yellow")) c = (Color32){255, 215, 90, 220};
        if (!strcmp(col, "orange")) c = (Color32){255, 150, 40, 220};
        if (!strcmp(col, "white"))  c = (Color32){255, 255, 255, 190};
        Vec3 zero = {0, 0, 0};
        fn_ps_emit5(g_sun_ps, pos, zero, size, 1.1f, c, mi_ps_emit5);
        /* внешнее свечение - большая слабая частица */
        Color32 halo = c; halo.a = 50;
        fn_ps_emit5(g_sun_ps, pos, zero, size * 2.4f, 1.1f, halo, mi_ps_emit5);
    }
}

/* ================= АТМОСФЕРНЫЕ ЧАСТИЦЫ (v1.10) =================
   Снег / лепестки / светлячки / дождь вокруг игрока.
   Тот же приём, что у солнца: ParticleSystem с выключенной эмиссией,
   частицы спавним вручную через Emit с заданной позицией/скоростью. */
static float amb_rand(void) {
    g_amb_rng = g_amb_rng * 1664525u + 1013904223u;
    return (float)(g_amb_rng >> 8) * (1.f / 16777216.f);
}

static void ambient_tick(void) {
    const char* mode = g_api->get_string(g_id, "ambient_fx", "off");
    int on = strcmp(mode, "off") != 0;
    if (!on) {
        if (g_amb_go && fn_go_setactive) { fn_go_setactive(g_amb_go, 0, mi_go_setactive); g_amb_ps = 0; }
        return;
    }
    if (g_amb_ps && !obj_alive(g_amb_ps)) {   /* умер при смене сцены - пересоздадим */
        g_amb_ps = 0; g_amb_go = 0;
    }
    if (!g_amb_ps) {
        if (!fn_obj_new || !g_go_klass || !fn_go_addcomponent || !g_ps_type || !fn_go_ctor) return;
        void* go = fn_obj_new(g_go_klass);
        void* nm = bearite_il2.string_new("fish_ambient");
        fn_go_ctor(go, nm, mi_go_ctor);
        g_amb_ps = fn_go_addcomponent(go, g_ps_type, mi_go_addcomponent);
        g_amb_go = go;
        gc_pin(go);   /* v1.10.2: защита от GC */
        if (!g_amb_ps) return;
        find_soft_particle_material();
        apply_soft_material(go);
        if (fn_ps_getemission && fn_em_set_enabled) {
            void* em_val = fn_ps_getemission(g_amb_ps, mi_ps_getemission);
            void* em_ptr = &em_val;
            fn_em_set_enabled(em_ptr, 0, mi_em_enabled);
        }
        g_api->log(g_id, "FishVisuals: AMBIENT создано");
    }
    if (!g_last_pos_valid || !fn_ps_emit5) return;

    if (g_time_t >= g_amb_next_emit) {
        g_amb_next_emit = g_time_t + 0.12f;
        /* v1.10.9: плотность (частиц за тик) и множитель размера из настроек */
        int count = (int)g_api->get_float(g_id, "ambient_density", 3.0);
        if (count < 1) count = 1;
        if (count > 12) count = 12;
        float szmul = (float)g_api->get_float(g_id, "ambient_size", 1.0);
        if (szmul < 0.2f) szmul = 0.2f;
        if (szmul > 4.f) szmul = 4.f;
        int is_snow     = !strcmp(mode, "snow");
        int is_petals   = !strcmp(mode, "petals");
        int is_firefly  = !strcmp(mode, "fireflies");
        int is_rain     = !strcmp(mode, "rain");
        for (int i = 0; i < count; i++) {
            Vec3 pos, vel;
            float size, life;
            Color32 c = {255, 255, 255, 200};
            float rx = (amb_rand() * 2.f - 1.f);
            float rz = (amb_rand() * 2.f - 1.f);
            if (is_snow) {
                pos = (Vec3){ g_last_pos.x + rx * 8.f,  g_last_pos.y + 5.f + amb_rand() * 4.f, g_last_pos.z + rz * 8.f };
                vel = (Vec3){ rx * 0.4f, -1.2f - amb_rand(), rz * 0.4f };
                size = 0.12f + amb_rand() * 0.14f; life = 9.f;
                c = (Color32){255, 255, 255, 200};
            } else if (is_petals) {
                pos = (Vec3){ g_last_pos.x + rx * 7.f,  g_last_pos.y + 4.f + amb_rand() * 4.f, g_last_pos.z + rz * 7.f };
                vel = (Vec3){ rx * 0.5f, -0.55f - amb_rand() * 0.4f, rz * 0.5f };
                size = 0.2f + amb_rand() * 0.18f; life = 8.f;
                c = (Color32){255, 165 + (uint8_t)(amb_rand() * 50.f), 195, 225};
            } else if (is_firefly) {
                pos = (Vec3){ g_last_pos.x + rx * 5.f,  g_last_pos.y + 0.4f + amb_rand() * 1.8f, g_last_pos.z + rz * 5.f };
                vel = (Vec3){ rx * 0.3f, 0.05f + amb_rand() * 0.15f, rz * 0.3f };
                size = 0.07f + amb_rand() * 0.09f; life = 5.f;
                c = (Color32){205, 255, 120, 130};
            } else if (is_rain) {
                pos = (Vec3){ g_last_pos.x + rx * 9.f,  g_last_pos.y + 6.f + amb_rand() * 3.f, g_last_pos.z + rz * 9.f };
                vel = (Vec3){ 0, -9.f, 0 };
                size = 0.07f + amb_rand() * 0.05f; life = 1.4f;
                c = (Color32){185, 205, 225, 110};
            } else break;
            fn_ps_emit5(g_amb_ps, pos, vel, size * szmul, life, c, mi_ps_emit5);
        }
    }
}

/* ================= ЗОЛОТОЙ МАРКЕР НАД ГОЛОВОЙ (v1.11.8) =================
   Вместо TMP-тега (крашил нативный рендер TMP на всех вариантах):
   светящаяся золотая точка с гalo над медведем. Работает на инфраструктуре
   частиц (солнце/атмосфера) - стабильной. */
static void* g_marker_ps;
static void* g_marker_go;
static float g_marker_next;

static void marker_tick(void) {
    int on = g_api->get_bool(g_id, "gold_marker", 0);
    if (!on) {
        if (g_marker_go && fn_go_setactive) { fn_go_setactive(g_marker_go, 0, mi_go_setactive); g_marker_ps = 0; }
        return;
    }
    if (g_marker_ps && !obj_alive(g_marker_ps)) { g_marker_ps = 0; g_marker_go = 0; }
    if (!g_marker_ps) {
        if (!fn_obj_new || !g_go_klass || !fn_go_addcomponent || !g_ps_type || !fn_go_ctor) return;
        void* go = fn_obj_new(g_go_klass);
        if (!go) return;
        void* nm = bearite_il2.string_new("fish_marker");
        fn_go_ctor(go, nm, mi_go_ctor);
        g_marker_ps = fn_go_addcomponent(go, g_ps_type, mi_go_addcomponent);
        g_marker_go = go;
        gc_pin(go);
        if (!g_marker_ps) return;
        if (fn_ps_getemission && fn_em_set_enabled) {
            void* em_val = fn_ps_getemission(g_marker_ps, mi_ps_getemission);
            void* em_ptr = &em_val;
            fn_em_set_enabled(em_ptr, 0, mi_em_enabled);
        }
        if (!g_soft_mat) find_soft_particle_material();
        if (g_soft_mat) apply_soft_material(go);
        g_api->log(g_id, "FishVisuals: MARKER создано");
    }
    if (!g_last_pos_valid || !fn_ps_emit5) return;
    if (g_time_t >= g_marker_next) {
        g_marker_next = g_time_t + 0.4f;
        float h = (float)g_api->get_float(g_id, "marker_height", 1.5);
        Vec3 pos = { g_last_pos.x, g_last_pos.y + h + 0.05f * sinf(g_time_t * 3.f), g_last_pos.z };
        Vec3 zero = {0, 0, 0};
        Color32 gold = {255, 210, 60, 230};
        fn_ps_emit5(g_marker_ps, pos, zero, 0.28f, 0.9f, gold, mi_ps_emit5);
        Color32 halo = {255, 190, 40, 60};
        fn_ps_emit5(g_marker_ps, pos, zero, 0.7f, 0.9f, halo, mi_ps_emit5);
    }
}

/* ================= ПЕРЕКРАСКА НЕБА v2 (v1.12) =================
   В SBA есть штатный SkyboxChanger (поле materialColorOverride @0x28 и
   метод ChangeSkyboxColor) - пользуемся родным механизмом игры вместо
   хаков с RenderSettings. Находим все экземпляры в сцене, запоминаем
   исходный цвет, красим/восстанавливаем. */
static void sky2_tick(void) {
    if (!fn_find_objects || !g_skych_type || !fn_skych_color) return;

    /* тот же "небесный" переключатель, что и у пресетов */
    const char* pname = g_api->get_string(g_id, "sky_preset", "off");
    int active = strcmp(pname, "off") != 0
              || g_api->get_bool(g_id, "disco_sky", 0)
              || g_api->get_bool(g_id, "sky_pulse", 0)
              || g_api->get_bool(g_id, "beauty_mode", 0);
    if (!active) {
        /* восстановление исходных цветов */
        for (int i = 0; i < 8; i++) {
            if (!g_skych_orig[i].ptr || !obj_alive(g_skych_orig[i].ptr)) { g_skych_orig[i].ptr = 0; continue; }
            fn_skych_color(g_skych_orig[i].ptr, g_skych_orig[i].orig, mi_skych_color);
            g_skych_orig[i].ptr = 0;
        }
        return;
    }

    void* arr = fn_find_objects(g_skych_type, mi_find_objects);
    if (!arr) return;
    int n = *(int*)((char*)arr + 0x18);
    if (n > 8) n = 8;
    void** items = (void**)((char*)arr + 0x20);

    Color c = preset_sky_color();
    float k = (float)g_api->get_float(g_id, "sky_intensity", 1.0);
    c.r *= k; c.g *= k; c.b *= k;

    for (int i = 0; i < n; i++) {
        void* inst = items[i];
        if (!inst || !obj_alive(inst)) continue;
        /* сохранить исходник один раз */
        int known = 0;
        for (int j = 0; j < 8; j++)
            if (g_skych_orig[j].ptr == inst) { known = 1; break; }
        if (!known) {
            for (int j = 0; j < 8; j++)
                if (!g_skych_orig[j].ptr) {
                    g_skych_orig[j].ptr = inst;
                    g_skych_orig[j].orig = *(Color*)((char*)inst + 0x28);
                    break;
                }
        }
        /* родной метод игры - обновит и _runtimeSkybox материал */
        fn_skych_color(inst, c, mi_skych_color);
    }
    if (!g_skych_logged) {
        g_skych_logged = 1;
        char b[96];
        snprintf(b, sizeof b, "FishVisuals: SkyboxChanger найден: %d", n);
        g_api->log(g_id, b);
    }
}

/* ---- обход дерева skyboxElements: красим материалы мешей неба ---- */
/* v1.16: HasProperty нельзя использовать - у него два перегруженных метода
   (string)/(int) с одинаковым argc, резолвер берёт неверный и всё врёт.
   Пишем ВСЕ кандидаты без проверки - запись в чужое свойство безобидна. */
static const char* kSkyProps[8] = { "_Color", "_BaseColor", "_TintColor", "_EmissionColor",
                                    "_SkyTint", "_GroundColor", "_Color1", "_Color2" };
#define SKY_PROP_COUNT 8

static void sky3_save_orig(void* mat, int pi, Color o) {
    for (int i = 0; i < 32; i++) {
        if (g_sky3_mats[i].mat == mat) {
            if (!(g_sky3_mats[i].saved & (1u << pi))) { g_sky3_mats[i].orig[pi] = o; g_sky3_mats[i].saved |= (1u << pi); }
            return;
        }
        if (!g_sky3_mats[i].mat) {
            g_sky3_mats[i].mat = mat;
            g_sky3_mats[i].orig[pi] = o;
            g_sky3_mats[i].saved = (1u << pi);
            return;
        }
    }
}

static void sky3_tint_mat(void* rend, Color c) {
    if (!rend || !fn_trail_getmaterial || !fn_mat_hasprop || !fn_mat_setcolor2 || !fn_mat_gettexture) return;
    void* mat = fn_trail_getmaterial(rend, mi_trail_getmaterial);
    if (!mat || !obj_alive(mat)) return;
    if (fn_mat_enablekw) {
        void* kw = bearite_il2.string_new("_EMISSION");
        if (kw) fn_mat_enablekw(mat, kw, mi_mat_enablekw);
    }
    for (int pi = 0; pi < SKY_PROP_COUNT; pi++) {
        void* pn = bearite_il2.string_new(kSkyProps[pi]);
        if (!pn) continue;
        int known = 0;
        for (int i = 0; i < 32; i++) if (g_sky3_mats[i].mat == mat) { known = 1; break; }
        if (!known && fn_mat_getcolor) {
            Color o = fn_mat_getcolor(mat, pn, mi_mat_getcolor);
            sky3_save_orig(mat, pi, o);
        }
        fn_mat_setcolor2(mat, pn, c, mi_mat_setcolor2);
    }
}

static void sky3_log_child(void* go, void* rend) {
    if (g_sky3_diag >= 8 || !fn_obj_getname || !il_string_chars || !fn_mat_getshader || !fn_shader_getname) return;
    void* nm = fn_obj_getname(go, mi_obj_getname);
    void* mat = fn_trail_getmaterial ? fn_trail_getmaterial(rend, mi_trail_getmaterial) : 0;
    void* shn = 0;
    if (mat && fn_mat_getshader && fn_shader_getname) {
        void* sh = fn_mat_getshader(mat, mi_mat_getshader);
        if (sh) shn = fn_shader_getname(sh, mi_shader_getname);
    }
    char nb[56] = "?", sb[72] = "?";
    if (nm && il_string_length) {
        int L = il_string_length(nm); if (L > 50) L = 50;
        const char* ch = il_string_chars(nm);
        for (int i = 0; i < L; i++) nb[i] = ch[i * 2];
        nb[L] = 0;
    }
    if (shn && il_string_length) {
        int L = il_string_length(shn); if (L > 65) L = 65;
        const char* ch = il_string_chars(shn);
        for (int i = 0; i < L; i++) sb[i] = ch[i * 2];
        sb[L] = 0;
    }
    /* какие цветовые свойства есть */
    int pmask = 0;
    if (mat && fn_mat_hasprop) {
        for (int pi = 0; pi < 4; pi++) {
            void* pn = bearite_il2.string_new(kSkyProps[pi]);
            if (pn && fn_mat_hasprop(mat, pn, mi_mat_hasprop)) pmask |= (1 << pi);
        }
    }
    char b[260];
    snprintf(b, sizeof b, "FishVisuals: SKYMESH[%d] obj='%s' shader='%s' props(_Color,_Base,_Tint,_Emis)=%d%d%d%d",
             g_sky3_diag, nb, sb, !!(pmask&1), !!(pmask&2), !!(pmask&4), !!(pmask&8));
    g_sky3_diag++;
    g_api->log(g_id, b);
}

static void sky3_tint_tree(void* tr, Color c, int depth) {
    if (!tr || depth > 6 || !fn_tr_childcount || !fn_tr_getchild || !fn_go_getcomponent) return;
    void* go = fn_get_gameobject ? fn_get_gameobject(tr, mi_get_gameobject) : 0;
    if (go) {
        void* rend = fn_go_getcomponent(go, g_meshrend_type, mi_go_getcomponent);
        if (!rend && g_skinrend_type) rend = fn_go_getcomponent(go, g_skinrend_type, mi_go_getcomponent);
        if (!rend && g_sprend_type)   rend = fn_go_getcomponent(go, g_sprend_type, mi_go_getcomponent);
        if (!rend && g_psrend_type)   rend = fn_go_getcomponent(go, g_psrend_type, mi_go_getcomponent);
        if (rend) { sky3_log_child(go, rend); sky3_tint_mat(rend, c); }
    }
    int n = fn_tr_childcount(tr, mi_tr_childcount);
    if (n > 32) n = 32;
    for (int i = 0; i < n; i++) {
        void* ch = fn_tr_getchild(tr, i, mi_tr_getchild);
        if (ch) sky3_tint_tree(ch, c, depth + 1);
    }
}

static void sky3_restore(void) {
    for (int i = 0; i < 32; i++) {
        void* mat = g_sky3_mats[i].mat;
        if (!mat) continue;
        if (!obj_alive(mat)) { g_sky3_mats[i].mat = 0; continue; }
        if (fn_mat_setcolor2) {
            for (int pi = 0; pi < SKY_PROP_COUNT; pi++) {
                if (!(g_sky3_mats[i].saved & (1u << pi))) continue;   /* не трогали - не трогаем */
                void* pn = bearite_il2.string_new(kSkyProps[pi]);
                if (pn) fn_mat_setcolor2(mat, pn, g_sky3_mats[i].orig[pi], mi_mat_setcolor2);
            }
        }
        g_sky3_mats[i].mat = 0;
        g_sky3_mats[i].saved = 0;
    }
}

/* v1.13: настоящий купол неба = меши под skyboxElements у CustomSkyboxTrigger (0x38)
   и CustomSkyboxEffects (0x30). Красим их материалы напрямую. */
static void sky3_tick(void) {
    int active = strcmp(g_api->get_string(g_id, "sky_preset", "off"), "off") != 0
              || g_api->get_bool(g_id, "disco_sky", 0)
              || g_api->get_bool(g_id, "sky_pulse", 0)
              || g_api->get_bool(g_id, "beauty_mode", 0);
    if (!active) { sky3_restore(); return; }
    if (!fn_find_objects || !g_meshrend_type) return;

    Color c = preset_sky_color();
    float k = (float)g_api->get_float(g_id, "sky_intensity", 1.0);
    c.r *= k; c.g *= k; c.b *= k;

    int found = 0;
    if (g_skytrig_type) {
        void* arr = fn_find_objects(g_skytrig_type, mi_find_objects);
        if (arr) {
            int n = *(int*)((char*)arr + 0x18); if (n > 16) n = 16;
            void** items = (void**)((char*)arr + 0x20);
            for (int i = 0; i < n; i++) {
                void* inst = items[i];
                if (!inst || !obj_alive(inst)) continue;
                void* tr = *(void**)((char*)inst + 0x38);   /* skyboxElements */
                if (tr) { sky3_tint_tree(tr, c, 0); found++; }
            }
        }
    }
    if (g_skyeff_type) {
        void* arr = fn_find_objects(g_skyeff_type, mi_find_objects);
        if (arr) {
            int n = *(int*)((char*)arr + 0x18); if (n > 16) n = 16;
            void** items = (void**)((char*)arr + 0x20);
            for (int i = 0; i < n; i++) {
                void* inst = items[i];
                if (!inst || !obj_alive(inst)) continue;
                void* tr = *(void**)((char*)inst + 0x30);   /* skyboxElements */
                if (tr) { sky3_tint_tree(tr, c, 0); found++; }
            }
        }
    }
    if (!g_sky3_logged) {
        g_sky3_logged = 1;
        char b[110];
        snprintf(b, sizeof b, "FishVisuals: skyboxElements найдено: %d", found);
        g_api->log(g_id, b);
    }
}

/* ---- ОХОТНИК ЗА НЕБОМ (v1.14): сканируем ВСЕ материалы сцены,
   ищем шейдер/объект со словами sky/cloud/dome/atmo - и красим.
   Найденное имя шейдера пишем в лог - по нему узнаем точное свойство цвета. ---- */
static int str_contains_ci(const char* hay, const char* needle) {
    if (!hay || !needle) return 0;
    char h[160], n[64];
    int i;
    for (i = 0; hay[i] && i < (int)sizeof(h) - 1; i++) h[i] = (hay[i] >= 'A' && hay[i] <= 'Z') ? hay[i] + 32 : hay[i];
    h[i] = 0;
    for (i = 0; needle[i] && i < (int)sizeof(n) - 1; i++) n[i] = (needle[i] >= 'A' && needle[i] <= 'Z') ? needle[i] + 32 : needle[i];
    n[i] = 0;
    return strstr(h, n) != 0;
}

static int il2_name_contains(void* ilstr, const char* needle) {
    if (!ilstr || !il_string_chars || !il_string_length) return 0;
    int len = il_string_length(ilstr);
    if (len <= 0 || len > 120) return 0;
    char buf[160];
    const char* ch = il_string_chars(ilstr);   /* UTF-16LE */
    for (int i = 0; i < len; i++) buf[i] = ch[i * 2];
    buf[len] = 0;
    return str_contains_ci(buf, needle);
}

static const char* kSkyWords[] = { "sky", "cloud", "dome", "atmo", "nebula", "horizon", 0 };

static void sky_hunt_tick(void) {
    int active = strcmp(g_api->get_string(g_id, "sky_preset", "off"), "off") != 0
              || g_api->get_bool(g_id, "disco_sky", 0)
              || g_api->get_bool(g_id, "sky_pulse", 0)
              || g_api->get_bool(g_id, "beauty_mode", 0);
    if (!active) return;
    static float g_hunt_next;
    if (g_time_t < g_hunt_next) return;
    g_hunt_next = g_time_t + 1.5f;
    if (!fn_find_objects || !g_meshrend_type || !fn_mat_getshader || !fn_shader_getname) return;

    Color c = preset_sky_color();
    float k = (float)g_api->get_float(g_id, "sky_intensity", 1.0);
    c.r *= k; c.g *= k; c.b *= k;

    void* arr = fn_find_objects(g_meshrend_type, mi_find_objects);
    if (!arr) return;
    int n = *(int*)((char*)arr + 0x18);
    if (n > 500) n = 500;
    void** items = (void**)((char*)arr + 0x20);
    static int g_hunt_logged;
    for (int i = 0; i < n; i++) {
        void* rend = items[i];
        if (!rend || !obj_alive(rend) || !fn_trail_getmaterial) continue;
        void* mat = fn_trail_getmaterial(rend, mi_trail_getmaterial);
        if (!mat || !obj_alive(mat)) continue;
        void* sh = fn_mat_getshader(mat, mi_mat_getshader);
        if (!sh) continue;
        void* shn = fn_shader_getname(sh, mi_shader_getname);
        int is_sky = il2_name_contains(shn, "sky") || il2_name_contains(shn, "cloud")
                  || il2_name_contains(shn, "dome") || il2_name_contains(shn, "atmo")
                  || il2_name_contains(shn, "nebula") || il2_name_contains(shn, "horizon");
        if (!is_sky) continue;
        sky3_tint_mat(rend, c);
        if (!g_hunt_logged) {
            g_hunt_logged = 1;
            char b[180];
            const char* nm = "";
            if (shn && il_string_chars) {
                int len = il_string_length(shn); if (len > 60) len = 60;
                static char sbuf[64];
                const char* ch = il_string_chars(shn);
                for (int j = 0; j < len; j++) sbuf[j] = ch[j * 2];
                sbuf[len] = 0;
                nm = sbuf;
            }
            snprintf(b, sizeof b, "FishVisuals: НЕБО найдено! shader='%s' (материалов: %d+)", nm, 1);
            g_api->log(g_id, b);
        }
    }
}

static void visuals_tick(void* player, float dt) {
    int    chroma_on  = g_api->get_bool(g_id, "enable_chroma", 0);
    int    trail_on   = g_api->get_bool(g_id, "enable_trail", 1);
    double chroma_spd = g_api->get_float(g_id, "chroma_speed", 2.0);
    int    beauty     = g_api->get_bool(g_id, "beauty_mode", 0);
    /* v1.11.1 SELFTEST: один тумблер форсит всё разом для диагностики */
    int    selftest   = g_api->get_bool(g_id, "selftest", 0);
    if (selftest) {
        chroma_on = 1;
        trail_on  = 1;
        beauty    = 0;
        chroma_spd = 0.0;   /* фиксированный красный */
    }
    if (beauty) { chroma_on = 1; trail_on = 1; }
    void* cosmetics = *(void**)((char*)player + 0x150);

    /* heartbeat под selftest: 30 секунд, потом молчит */
    if (selftest) {
        static float g_st_next;
        static int   g_st_count;
        if (g_st_count < 30 && g_time_t >= g_st_next) {
            g_st_next = g_time_t + 1.f;
            g_st_count++;
            void* m1s = cosmetics ? *(void**)((char*)cosmetics + 0x20) : 0;
            Color rc = {0,0,0,0};
            if (m1s && fn_mat_getcolor_prop) rc = fn_mat_getcolor_prop(m1s, mi_mat_getcolor_prop);
            Vec3 hs = {0,0,0};
            void* hd = cosmetics ? *(void**)((char*)cosmetics + 0x38) : 0;
            if (hd && fn_tr_get_scale) hs = fn_tr_get_scale(hd, mi_tr_get_scale);
            char b[320];
            snprintf(b, sizeof b, "FishVisuals: ST%d cos=%p m1=%p rgb=%.2f/%.2f/%.2f headscale=%.2f trail=%p",
                     g_st_count, cosmetics, m1s, rc.r, rc.g, rc.b, hs.x, g_trail);
            g_api->log(g_id, b);
        }
    }

    /* хрома-скин */
    if (chroma_on && cosmetics && fn_set_color) {
        g_hue += 0.15f * (float)chroma_spd * dt;
        if (g_hue > 1.f) g_hue -= 1.f;
        /* стробоскоп: hue перескакивает дискретно, без плавного перехода */
        float eff_hue = g_api->get_bool(g_id, "chroma_strobe", 0)
            ? (float)(((int)(g_hue * 12.f)) % 12) / 12.f
            : g_hue;
        Color c = hsv2rgb(eff_hue);
        void* m1 = *(void**)((char*)cosmetics + 0x20);
        void* m2 = *(void**)((char*)cosmetics + 0x28);
        /* v1.9.3: запоминаем исходные цвета ОДИН раз, чтобы выключение
           вернуло скину нормальный цвет, а не замороженный радужный */
        if (fn_mat_getcolor_prop && !g_chroma_saved) {
            if (m1) g_skin_orig[0] = fn_mat_getcolor_prop(m1, mi_mat_getcolor_prop);
            if (m2) g_skin_orig[1] = fn_mat_getcolor_prop(m2, mi_mat_getcolor_prop);
            if (m1 || m2) g_chroma_saved = 1;
        }
        if (m1) fn_set_color(m1, c, mi_set_color);
        if (m2) fn_set_color(m2, c, mi_set_color);
        /* v1.11.1: страховка - кастомные шейдеры SBA могут читать цвет из _BaseColor */
        if (fn_mat_setcolor2) {
            void* bn = bearite_il2.string_new("_BaseColor");
            if (bn) {
                if (m1) fn_mat_setcolor2(m1, bn, c, mi_mat_setcolor2);
                if (m2) fn_mat_setcolor2(m2, bn, c, mi_mat_setcolor2);
            }
        }
        if (!g_dbg_chroma && (m1 || m2)) {
            g_dbg_chroma = 1;
            char b[128];
            snprintf(b, sizeof b, "FishVisuals: CHROMA applied m1=%p m2=%p", m1, m2);
            g_api->log(g_id, b);
        }
    }

    /* v1.9.3: хрома выключена - возвращаем скину исходные цвета */
    if (!chroma_on && g_chroma_saved && fn_set_color) {
        void* m1 = cosmetics ? *(void**)((char*)cosmetics + 0x20) : 0;
        void* m2 = cosmetics ? *(void**)((char*)cosmetics + 0x28) : 0;
        if (m1) fn_set_color(m1, g_skin_orig[0], mi_set_color);
        if (m2) fn_set_color(m2, g_skin_orig[1], mi_set_color);
        g_chroma_saved = 0;
    }

    /* свой aspect */
    if ((g_tick % 32) == 0 && g_api->get_bool(g_id, "enable_aspect", 0)
        && fn_cam_main && orig_set_aspect) {
        void* cam = fn_cam_main(mi_cam_main);
        if (cam) orig_set_aspect(cam, (float)g_api->get_float(g_id, "aspect_value", 1.7778), mi_cam_aspect);
    }

    /* скорость игрока v1.3: для динамического FOV (по world-позиции контроллера) */
    void* ctrl_tr_v = fn_get_transform ? fn_get_transform(player, mi_get_transform) : 0;
    if (ctrl_tr_v && fn_tr_get_position) {
        Vec3 pos = fn_tr_get_position(ctrl_tr_v, mi_tr_get_position);
        if (g_last_pos_valid && dt > 0.0001f) {
            float dx = pos.x - g_last_pos.x, dy = pos.y - g_last_pos.y, dz = pos.z - g_last_pos.z;
            float inst = sqrtf(dx*dx + dy*dy + dz*dz) / dt;
            g_speed += (inst - g_speed) * 0.25f;   /* сглаживание */
        }
        g_last_pos = pos;
        g_last_pos_valid = 1;
    }

    /* slo-mo / ускорение (Time.timeScale) + пульс времени v1.3 */
    g_time_t += dt;
    int ts_on = g_api->get_bool(g_id, "enable_timescale", 0);
    int tp_on = g_api->get_bool(g_id, "time_pulse", 0);
    if (ts_on && fn_time_scale) {
        fn_time_scale((float)g_api->get_float(g_id, "timescale_value", 1.0), mi_time_scale);
        g_ts_applied = 1;
    } else if (tp_on && fn_time_scale) {
        float amp = (float)g_api->get_float(g_id, "time_pulse_amp", 0.25);
        fn_time_scale(1.f + amp * sinf(g_time_t * 3.f), mi_time_scale);
        g_ts_applied = 1;
    } else if (g_ts_applied && fn_time_scale) {
        fn_time_scale(1.f, mi_time_scale);
        g_ts_applied = 0;
    }

    crazy_tick(player, cosmetics, dt);

    if (trail_on) {
        ensure_trail(player);
        /* ФИКС v1.2: материал назначаем заново после пересоздания трейла */
        if (g_trail && !g_trail_mat_done && cosmetics && fn_trail_setmaterial) {
            void* mat = make_trail_material(cosmetics);
            if (mat) {
                fn_trail_setmaterial(g_trail, mat, mi_trail_setmaterial);
                g_api->log(g_id, "FishVisuals: material assigned to trail");
            }
            if (fn_trail_corners) fn_trail_corners(g_trail, 6, mi_trail_corners);
            if (fn_trail_caps)    fn_trail_caps(g_trail, 4, mi_trail_caps);
            g_trail_mat_done = 1;
        }
        /* ФИКС v1.2: каждый сеттер под своим null-check (MISS метода больше не крашит) */
        if (g_trail) {
            float t = (float)g_api->get_float(g_id, "trail_time", 1.0);
            float w = (float)g_api->get_float(g_id, "trail_width", 0.5);
            if (g_api->get_bool(g_id, "selftest", 0)) { w = 1.0f; t = 1.5f; }   /* жирный зелёный трейл */
            const char* tex = g_api->get_string(g_id, "trail_texture", "bubbles");
            if (beauty) {
                tex = "rainbow";
                if (w < 0.8f) w = 0.8f;
                if (t < 1.2f) t = 1.2f;
            }
            Color c = g_api->get_bool(g_id, "selftest", 0) ? (Color){0.2f, 1.f, 0.3f, 1.f} : trail_color(tex);
            Color e = c; e.a = 0.05f;
            int smooth = g_api->get_bool(g_id, "trail_smooth", 1);
            if (fn_trail_mvd)         fn_trail_mvd(g_trail, smooth ? 0.03f : 0.25f, mi_trail_mvd);
            if (fn_trail_time)        fn_trail_time(g_trail, t, mi_trail_time);
            if (fn_trail_startwidth)  fn_trail_startwidth(g_trail, w, mi_trail_startwidth);
            if (fn_trail_endwidth)    fn_trail_endwidth(g_trail, 0.02f, mi_trail_endwidth);
            if (fn_trail_startcolor)  fn_trail_startcolor(g_trail, c, mi_trail_startcolor);
            if (fn_trail_endcolor)    fn_trail_endcolor(g_trail, e, mi_trail_endcolor);
        }
    } else {
        /* v1.9.3: трейл выключен - УНИЧТОЖАЕМ компонент (Destroy отложен до конца
           кадра Unity, безопасно), чтобы на игроке ничего не осталось */
        if (g_trail && fn_object_destroy)
            fn_object_destroy(g_trail, mi_object_destroy);
        g_trail = 0;
        g_trail_last = 0;
        g_trail_go = 0;
        g_trail_mat_done = 0;
        g_trail_add_cd = 0;
    }

    cine_tick(dt);
    orbit_tick(dt);
    grade_tick();
    light_tick();
    sun_tick();
    ambient_tick();
    marker_tick();
    sky2_tick();
    sky3_tick();
    sky_hunt_tick();


}

static void (*orig_after_update)(void*, float, const void*);
static int g_dbg_hook;
static void* g_player_self;   /* v1.6.5: ЕДИНСТВЕННЫЙ локальный контроллер */

static void hk_after_update(void* self, float dt, const void* mi) {
    if (!self) return;                       /* ФИКС v1.2: было - оригинал вызывался с NULL */
    orig_after_update(self, dt, mi);
    g_tick++;
    if (*(uint8_t*)((char*)self + 0x138) == 0) return;
    /* v1.6.5: если уже привязались к локальному игроку - игнорим остальных
       (клоны контроллера/превью тоже могут иметь флаг isLocalPlayer) */
    /* v1.9.1: привязываемся ТОЛЬКО к контроллеру с валидным cosmetics.
       Пустые "локальные" объекты (превью/призраки/клоны) пропускаем,
       иначе вся покраска уходит в никуда. После смерти - перепривязка. */
    void* cos0 = *(void**)((char*)self + 0x150);
    if (g_player_self) {
        if (self != g_player_self) return;
        if (!cos0) { g_player_self = 0; return; }   /* умер - ищем нового */
    } else {
        if (!cos0) return;   /* не тот объект, ждём настоящего игрока */
        g_player_self = self;
        char b[160];
        snprintf(b, sizeof b, "FishVisuals: PLAYER bound self=%p cosmetics=%p", self, cos0);
        g_api->log(g_id, b);
    }
    if (!g_dbg_hook) {
        g_dbg_hook = 1;
        g_api->log(g_id, "FishVisuals: HOOK ALIVE - локальный игрок найден, тикаю");
    }
    visuals_tick(self, dt);
}

static void (*orig_set_fov)(void*, float, const void*);
static float g_fov_t;

static void hk_set_aspect(void* self, float value, const void* mi) {
    if (g_api->get_bool(g_id, "enable_aspect", 0))
        value = (float)g_api->get_float(g_id, "aspect_value", 1.7778);
    orig_set_aspect(self, value, mi);
}

static void hk_set_fov(void* self, float value, const void* mi) {
    if (g_api->get_bool(g_id, "drunk_fov", 0)) {
        g_fov_t += 0.016f;
        float base = (float)g_api->get_float(g_id, "custom_fov", 75.0);
        value = base + sinf(g_fov_t * 2.1f) * 14.f + sinf(g_fov_t * 5.7f) * 5.f;
    } else if (g_api->get_bool(g_id, "enable_fov", 0) || g_api->get_bool(g_id, "selftest", 0)) {
        value = g_api->get_bool(g_id, "selftest", 0) ? 100.f : (float)g_api->get_float(g_id, "custom_fov", 75.0);
    }
    /* v1.6: пульс FOV */
    if (g_api->get_bool(g_id, "fov_pulse", 0)) {
        float amp = (float)g_api->get_float(g_id, "fov_pulse_amp", 6.0);
        value += amp * sinf(g_time_t * 4.f);
    }
    /* v1.3: динамический FOV - расширяется от скорости бега */
    if (g_api->get_bool(g_id, "dyn_fov", 0)) {
        float boost = g_speed * 1.6f;
        float cap = (float)g_api->get_float(g_id, "dyn_fov_max", 20.0);
        if (boost > cap) boost = cap;
        value += boost;
    }
    orig_set_fov(self, value, mi);
}

__attribute__((visibility("default")))
void bearite_mod_init(const BeariteApi* api, const char* mod_id) {
    g_api = api;
    g_id  = mod_id;
    if (api->version != BEARITE_API_VERSION) return;
    api->log(mod_id, "FishVisuals v1.16: загрузка (SBA 13.0.8)");
    api->log(mod_id, "FishVisuals: STAGE 1 - резолв методов");

    if (!bearite_il2cpp_init(api)) {
        api->log(mod_id, "FishVisuals: il2cpp API не найден, мод не работает");
        return;
    }
    il_class_get_type  = (void*(*)(void*))api->il2cpp_symbol("il2cpp_class_get_type");
    il_type_get_object = (void*(*)(void*))api->il2cpp_symbol("il2cpp_type_get_object");

    mi_set_color         = bearite_find_method("UnityEngine", "Material",  "set_color", 1);
    mi_get_gameobject    = bearite_find_method("UnityEngine", "Component", "get_gameObject", 0);
    mi_go_getcomponent   = bearite_find_method("UnityEngine", "GameObject", "GetComponent", 1);
    mi_go_addcomponent   = bearite_find_method("UnityEngine", "GameObject", "AddComponent", 1);
    mi_trail_time        = bearite_find_method("UnityEngine", "TrailRenderer", "set_time", 1);
    mi_trail_startwidth  = bearite_find_method("UnityEngine", "TrailRenderer", "set_startWidth", 1);
    mi_trail_endwidth    = bearite_find_method("UnityEngine", "TrailRenderer", "set_endWidth", 1);
    mi_trail_startcolor  = bearite_find_method("UnityEngine", "TrailRenderer", "set_startColor", 1);
    mi_trail_endcolor    = bearite_find_method("UnityEngine", "TrailRenderer", "set_endColor", 1);
    mi_trail_mvd         = bearite_find_method("UnityEngine", "TrailRenderer", "set_minVertexDistance", 1);
    mi_shader_find       = bearite_find_method("UnityEngine", "Shader",   "Find", 1);
    mi_object_instantiate= bearite_find_method("UnityEngine", "Object",   "Instantiate", 1);
    mi_mat_setshader     = bearite_find_method("UnityEngine", "Material", "set_shader", 1);
    mi_trail_setmaterial = bearite_find_method("UnityEngine", "Renderer", "set_material", 1);
    mi_trail_getmaterial = bearite_find_method("UnityEngine", "Renderer", "get_material", 0);
    mi_tr_set_euler  = bearite_find_method("UnityEngine", "Transform", "set_localEulerAngles", 1);
    mi_tr_set_scale  = bearite_find_method("UnityEngine", "Transform", "set_localScale", 1);
    mi_set_sky       = bearite_find_method("UnityEngine", "RenderSettings", "set_ambientSkyColor", 1);
    mi_set_equator   = bearite_find_method("UnityEngine", "RenderSettings", "set_ambientEquatorColor", 1);
    mi_set_ground    = bearite_find_method("UnityEngine", "RenderSettings", "set_ambientGroundColor", 1);
    mi_tr_get_euler  = bearite_find_method("UnityEngine", "Transform", "get_localEulerAngles", 0);
    mi_trail_corners = bearite_find_method("UnityEngine", "TrailRenderer", "set_numCornerVertices", 1);
    mi_trail_caps    = bearite_find_method("UnityEngine", "TrailRenderer", "set_numCapVertices", 1);
    mi_mat_setmaintex= bearite_find_method("UnityEngine", "Material", "set_mainTexture", 1);
    mi_cam_aspect    = bearite_find_method("UnityEngine", "Camera", "set_aspect", 1);
    mi_cam_main      = bearite_find_method("UnityEngine", "Camera", "get_main", 0);
    mi_get_transform = bearite_find_method("UnityEngine", "Component", "get_transform", 0);
    mi_tr_get_scale  = bearite_find_method("UnityEngine", "Transform", "get_localScale", 0);
    mi_time_scale    = bearite_find_method("UnityEngine", "Time", "set_timeScale", 1);
    mi_tr_get_position = bearite_find_method("UnityEngine", "Transform", "get_position", 0);
    mi_set_ambmode     = bearite_find_method("UnityEngine", "RenderSettings", "set_ambientMode", 1);
    mi_get_skybox      = bearite_find_method("UnityEngine", "RenderSettings", "get_skybox", 0);
    mi_mat_setcolor2   = bearite_find_method("UnityEngine", "Material", "SetColor", 2);
    mi_mat_getcolor    = bearite_find_method("UnityEngine", "Material", "GetColor", 1);
    mi_mat_hasprop     = bearite_find_method("UnityEngine", "Material", "HasProperty", 1);
    mi_tr_set_position = bearite_find_method("UnityEngine", "Transform", "set_position", 1);
    mi_tr_set_rotation = bearite_find_method("UnityEngine", "Transform", "set_rotation", 1);
    mi_set_enabled     = bearite_find_method("UnityEngine", "Behaviour", "set_enabled", 1);
    mi_go_ctor         = bearite_find_method("UnityEngine", "GameObject", ".ctor", 1);
    fn_obj_new         = (alloc_obj_fn)api->il2cpp_symbol("il2cpp_object_new");
    il_class_get_nested = (void*(*)(void*, void**))api->il2cpp_symbol("il2cpp_class_get_nested_types");
    il_gchandle_new    = (unsigned(*)(void*, int))api->il2cpp_symbol("il2cpp_gchandle_new");
    il_array_new       = (void*(*)(void*, int))api->il2cpp_symbol("il2cpp_array_new");
    il_array_class_get = (void*(*)(void*, unsigned))api->il2cpp_symbol("il2cpp_array_class_get");
    il_class_get_name  = (const char*(*)(void*))api->il2cpp_symbol("il2cpp_class_get_name");
    g_go_klass         = bearite_find_class("UnityEngine", "GameObject");
    mi_go_setactive    = bearite_find_method("UnityEngine", "GameObject", "SetActive", 1);
    mi_tr_set_parent   = bearite_find_method("UnityEngine", "Transform", "set_parent", 1);
    mi_cam_replshader  = bearite_find_method("UnityEngine", "Camera", "SetReplacementShader", 2);
    mi_cam_resetshader = bearite_find_method("UnityEngine", "Camera", "ResetReplacementShader", 0);
    mi_set_clearflags  = bearite_find_method("UnityEngine", "Camera", "set_clearFlags", 1);
    mi_set_bgcolor     = bearite_find_method("UnityEngine", "Camera", "set_backgroundColor", 1);
    mi_find_objects    = bearite_find_method("UnityEngine", "Object", "FindObjectsOfTypeAll", 1);
    mi_light_color     = bearite_find_method("UnityEngine", "Light", "set_color", 1);
    mi_light_getcolor  = bearite_find_method("UnityEngine", "Light", "get_color", 0);
    mi_set_fog         = bearite_find_method("UnityEngine", "RenderSettings", "set_fog", 1);
    api->log(mod_id, "FishVisuals: STAGE 2 - методы найдены, беру код");

    fn_set_color         = (set_color_fn)bearite_method_code(mi_set_color);
    fn_get_gameobject    = (get_obj_fn)bearite_method_code(mi_get_gameobject);
    fn_go_getcomponent   = (go_type_fn)bearite_method_code(mi_go_getcomponent);
    fn_go_addcomponent   = (go_type_fn)bearite_method_code(mi_go_addcomponent);
    fn_trail_time        = (set_float_fn)bearite_method_code(mi_trail_time);
    fn_trail_startwidth  = (set_float_fn)bearite_method_code(mi_trail_startwidth);
    fn_trail_endwidth    = (set_float_fn)bearite_method_code(mi_trail_endwidth);
    fn_trail_startcolor  = (set_color_fn)bearite_method_code(mi_trail_startcolor);
    fn_trail_endcolor    = (set_color_fn)bearite_method_code(mi_trail_endcolor);
    fn_trail_mvd         = (set_float_fn)bearite_method_code(mi_trail_mvd);
    fn_shader_find       = (shader_find_fn)bearite_method_code(mi_shader_find);
    fn_object_instantiate= (instantiate_fn)bearite_method_code(mi_object_instantiate);
    fn_mat_setshader     = (set_shader_fn)bearite_method_code(mi_mat_setshader);
    fn_trail_setmaterial = (set_material_fn)bearite_method_code(mi_trail_setmaterial);
    fn_trail_getmaterial = (get_obj_fn)bearite_method_code(mi_trail_getmaterial);
    fn_tr_set_euler  = (tr_vec3_fn)bearite_method_code(mi_tr_set_euler);
    fn_tr_set_scale  = (tr_vec3_fn)bearite_method_code(mi_tr_set_scale);
    fn_set_sky       = (set_ambient_fn)bearite_method_code(mi_set_sky);
    fn_set_equator   = (set_ambient_fn)bearite_method_code(mi_set_equator);
    fn_set_ground    = (set_ambient_fn)bearite_method_code(mi_set_ground);
    fn_tr_get_euler  = (get_euler_fn)bearite_method_code(mi_tr_get_euler);
    fn_trail_corners = (set_int_fn)bearite_method_code(mi_trail_corners);
    fn_trail_caps    = (set_int_fn)bearite_method_code(mi_trail_caps);
    fn_mat_setmaintex= (set_material_fn)bearite_method_code(mi_mat_setmaintex);
    fn_cam_main      = (static_obj_fn)bearite_method_code(mi_cam_main);
    fn_get_transform = (get_obj_fn)bearite_method_code(mi_get_transform);
    fn_tr_get_scale  = (get_vec3_fn)bearite_method_code(mi_tr_get_scale);
    fn_time_scale    = (static_float_fn)bearite_method_code(mi_time_scale);
    fn_tr_get_position = (get_pos_fn)bearite_method_code(mi_tr_get_position);
    fn_set_ambmode     = (static_int_fn)bearite_method_code(mi_set_ambmode);
    fn_get_skybox      = (static_obj_fn)bearite_method_code(mi_get_skybox);
    fn_mat_setcolor2   = (mat_setcolor2_fn)bearite_method_code(mi_mat_setcolor2);
    fn_mat_getcolor    = (mat_getcolor_fn)bearite_method_code(mi_mat_getcolor);
    fn_mat_hasprop     = (mat_hasprop_fn)bearite_method_code(mi_mat_hasprop);
    fn_tr_set_position = (set_pos_fn)bearite_method_code(mi_tr_set_position);
    fn_tr_set_rotation = (set_rot_fn)bearite_method_code(mi_tr_set_rotation);
    fn_set_enabled     = (set_enabled_fn)bearite_method_code(mi_set_enabled);
    fn_go_ctor         = (go_ctor_fn)bearite_method_code(mi_go_ctor);
    fn_go_setactive    = (go_setactive_fn)bearite_method_code(mi_go_setactive);
    fn_tr_set_parent    = (set_parent_fn)bearite_method_code(mi_tr_set_parent);
    fn_cam_replshader  = (cam_replshader_fn)bearite_method_code(mi_cam_replshader);
    fn_cam_resetshader = (cam_resetshader_fn)bearite_method_code(mi_cam_resetshader);
    fn_cam_set_clearflags = (set_clearflags_fn)bearite_method_code(mi_set_clearflags);
    fn_cam_set_bgcolor = (set_bgcolor_fn)bearite_method_code(mi_set_bgcolor);
    fn_find_objects    = (find_objects_fn)bearite_method_code(mi_find_objects);
    fn_light_color     = (light_set_color_fn)bearite_method_code(mi_light_color);
    fn_light_getcolor  = (light_get_color_fn)bearite_method_code(mi_light_getcolor);
    fn_set_fog         = (static_bool_fn)bearite_method_code(mi_set_fog);
    api->log(mod_id, "FishVisuals: STAGE 3 - код методов получен");

    api->log(mod_id, "FishVisuals: STAGE 4 - резолв типов (CinemachineBrain/Light/TrailRenderer)");
    void* brain_klass = bearite_find_class("Cinemachine", "CinemachineBrain");
    if (brain_klass && il_class_get_type && il_type_get_object)
        g_brain_type = il_type_get_object(il_class_get_type(brain_klass));
    if (!g_brain_type) api->log(mod_id, "FishVisuals: System.Type CinemachineBrain не получен, кинокамера недоступна");

    void* ps_klass = bearite_find_class("UnityEngine", "ParticleSystem");
    if (ps_klass && il_class_get_type && il_type_get_object)
        g_ps_type = il_type_get_object(il_class_get_type(ps_klass));
    mi_tr_childcount   = bearite_find_method("UnityEngine", "Transform", "get_childCount", 0);
    mi_tr_getchild     = bearite_find_method("UnityEngine", "Transform", "GetChild", 1);
    mi_mat_enablekw    = bearite_find_method("UnityEngine", "Material", "EnableKeyword", 1);
    mi_mat_getshader   = bearite_find_method("UnityEngine", "Material", "get_shader", 0);
    mi_shader_getname  = bearite_find_method("UnityEngine", "Shader", "get_name", 0);
    fn_mat_getshader   = (get_obj_fn)bearite_method_code(mi_mat_getshader);
    fn_shader_getname  = (get_obj_fn)bearite_method_code(mi_shader_getname);
    il_string_chars    = (const char*(*)(void*))api->il2cpp_symbol("il2cpp_string_chars");
    il_string_length   = (int(*)(void*))api->il2cpp_symbol("il2cpp_string_length");
    fn_tr_childcount   = (int(*)(void*, const void*))bearite_method_code(mi_tr_childcount);
    fn_tr_getchild     = (void*(*)(void*, int, const void*))bearite_method_code(mi_tr_getchild);
    fn_mat_enablekw    = (void(*)(void*, void*, const void*))bearite_method_code(mi_mat_enablekw);
    void* meshrend_klass = bearite_find_class("UnityEngine", "MeshRenderer");
    if (meshrend_klass && il_class_get_type && il_type_get_object)
        g_meshrend_type = il_type_get_object(il_class_get_type(meshrend_klass));
    void* skinrend_klass = bearite_find_class("UnityEngine", "SkinnedMeshRenderer");
    if (skinrend_klass && il_class_get_type && il_type_get_object)
        g_skinrend_type = il_type_get_object(il_class_get_type(skinrend_klass));
    void* sprend_klass = bearite_find_class("UnityEngine", "SpriteRenderer");
    if (sprend_klass && il_class_get_type && il_type_get_object)
        g_sprend_type = il_type_get_object(il_class_get_type(sprend_klass));
    mi_obj_getname     = bearite_find_method("UnityEngine", "Object", "get_name", 0);
    fn_obj_getname     = (get_obj_fn)bearite_method_code(mi_obj_getname);
    void* skytrig_klass = bearite_find_class("", "CustomSkyboxTrigger");
    if (skytrig_klass && il_class_get_type && il_type_get_object)
        g_skytrig_type = il_type_get_object(il_class_get_type(skytrig_klass));
    void* skyeff_klass = bearite_find_class("", "CustomSkyboxEffects");
    if (skyeff_klass && il_class_get_type && il_type_get_object)
        g_skyeff_type = il_type_get_object(il_class_get_type(skyeff_klass));

    void* skych_klass = bearite_find_class("", "SkyboxChanger");
    if (skych_klass && il_class_get_type && il_type_get_object)
        g_skych_type = il_type_get_object(il_class_get_type(skych_klass));
    mi_skych_color = bearite_find_method("", "SkyboxChanger", "ChangeSkyboxColor", 1);
    fn_skych_color = (void(*)(void*, Color, const void*))bearite_method_code(mi_skych_color);

    void* psrend_klass = bearite_find_class("UnityEngine", "ParticleSystemRenderer");
    if (psrend_klass && il_class_get_type && il_type_get_object)
        g_psrend_type = il_type_get_object(il_class_get_type(psrend_klass));
    if (ps_klass && il_class_get_nested && il_class_get_name) {
        void* iter = 0, *nc;
        while ((nc = il_class_get_nested(ps_klass, &iter))) {
            const char* nn = il_class_get_name(nc);
            if (nn && !strcmp(nn, "EmissionModule")) g_em_klass = nc;
        }
    }
    mi_mat_getcolor_prop = bearite_find_method("UnityEngine", "Material", "get_color", 0);
    mi_tex_ctor        = bearite_find_method("UnityEngine", "Texture2D", ".ctor", 4);
    mi_tex_loadimage   = bearite_find_method("UnityEngine", "ImageConversion", "LoadImage", 2);
    fn_tex_ctor        = (void(*)(void*, int, int, int, int, const void*))bearite_method_code(mi_tex_ctor);
    fn_tex_loadimage   = (int(*)(void*, void*, const void*))bearite_method_code(mi_tex_loadimage);
    g_byte_klass       = bearite_find_class("System", "Byte");
    mi_mat_getmaintex  = bearite_find_method("UnityEngine", "Material", "get_mainTexture", 0);
    mi_mat_gettexture  = bearite_find_method("UnityEngine", "Material", "GetTexture", 1);
    fn_mat_getmaintex  = (get_obj_fn)bearite_method_code(mi_mat_getmaintex);
    fn_mat_gettexture  = (mat_get_texture_fn)bearite_method_code(mi_mat_gettexture);
    mi_object_destroy  = bearite_find_method("UnityEngine", "Object", "Destroy", 1);
    fn_mat_getcolor_prop = (Color(*)(void*, const void*))bearite_method_code(mi_mat_getcolor_prop);
    fn_object_destroy  = (static_destroy_fn)bearite_method_code(mi_object_destroy);
    mi_tmp_settext     = bearite_find_method("TMPro", "TMP_Text", "set_text", 1);
    mi_tm_settext      = bearite_find_method("UnityEngine", "TextMesh", "set_text", 1);
    mi_tr_get_rotation = bearite_find_method("UnityEngine", "Transform", "get_rotation", 0);
    fn_tm_settext      = (tmp_set_text_fn)bearite_method_code(mi_tm_settext);
    fn_tr_get_rotation = (get_rot_fn)bearite_method_code(mi_tr_get_rotation);
    mi_tmp_setcolor    = bearite_find_method("TMPro", "TMP_Text", "set_color", 1);
    fn_tmp_settext     = (tmp_set_text_fn)bearite_method_code(mi_tmp_settext);
    fn_tmp_setcolor    = (tmp_set_color_fn)bearite_method_code(mi_tmp_setcolor);
    mi_ps_getemission  = bearite_find_method("UnityEngine", "ParticleSystem", "get_emission", 0);
    mi_ps_emit5        = bearite_find_method("UnityEngine", "ParticleSystem", "Emit", 5);
    mi_em_enabled      = g_em_klass ? bearite_il2.class_get_method_from_name(g_em_klass, "set_enabled", 1) : 0;
    fn_ps_getemission  = (ps_get_emission_fn)bearite_method_code(mi_ps_getemission);
    fn_ps_emit5        = (ps_emit5_fn)bearite_method_code(mi_ps_emit5);
    fn_em_set_enabled  = (em_set_enabled_fn)bearite_method_code(mi_em_enabled);

    void* light_klass = bearite_find_class("UnityEngine", "Light");
    if (light_klass && il_class_get_type && il_type_get_object)
        g_light_type = il_type_get_object(il_class_get_type(light_klass));

    void* trail_klass = bearite_find_class("UnityEngine", "TrailRenderer");
    if (trail_klass && il_class_get_type && il_type_get_object)
        g_trail_type = il_type_get_object(il_class_get_type(trail_klass));
    if (!g_trail_type) api->log(mod_id, "FishVisuals: System.Type TrailRenderer не получен, трейл недоступен");

    api->log(mod_id, fn_time_scale ? "FishVisuals: OK Time.set_timeScale"
                                   : "FishVisuals: MISS Time.set_timeScale");
    api->log(mod_id, fn_tr_get_position ? "FishVisuals: OK Transform.get_position"
                                        : "FishVisuals: MISS Transform.get_position");
    api->log(mod_id, fn_set_ambmode ? "FishVisuals: OK RenderSettings.set_ambientMode"
                                    : "FishVisuals: MISS RenderSettings.set_ambientMode");
    api->log(mod_id, fn_get_skybox ? "FishVisuals: OK RenderSettings.get_skybox"
                                   : "FishVisuals: MISS RenderSettings.get_skybox");
    api->log(mod_id, fn_mat_setcolor2 ? "FishVisuals: OK Material.SetColor"
                                      : "FishVisuals: MISS Material.SetColor");
    api->log(mod_id, fn_cam_replshader ? "FishVisuals: OK Camera.SetReplacementShader"
                                       : "FishVisuals: MISS Camera.SetReplacementShader");
    api->log(mod_id, fn_go_ctor ? "FishVisuals: OK GameObject..ctor"
                                : "FishVisuals: MISS GameObject..ctor");
    api->log(mod_id, fn_tr_set_parent ? "FishVisuals: OK Transform.set_parent"
                                      : "FishVisuals: MISS Transform.set_parent");
    api->log(mod_id, fn_find_objects ? "FishVisuals: OK Object.FindObjectsOfTypeAll"
                                     : "FishVisuals: MISS Object.FindObjectsOfTypeAll");
    api->log(mod_id, fn_light_color ? "FishVisuals: OK Light.set_color"
                                    : "FishVisuals: MISS Light.set_color");
    api->log(mod_id, fn_set_fog ? "FishVisuals: OK RenderSettings.set_fog"
                                : "FishVisuals: MISS RenderSettings.set_fog");
    api->log(mod_id, (fn_ps_emit5 && fn_em_set_enabled && g_ps_type)
             ? "FishVisuals: OK sun (Emit + EmissionModule)"
             : "FishVisuals: MISS sun (Emit/EmissionModule/Type)");
    api->log(mod_id, il_gchandle_new ? "FishVisuals: OK gchandle (GC-защита)"
                                     : "FishVisuals: MISS gchandle - частицы умрут через ~40с!");
    api->log(mod_id, (fn_skych_color && g_skych_type) ? "FishVisuals: OK SkyboxChanger"
                                                      : "FishVisuals: MISS SkyboxChanger");
    api->log(mod_id, fn_trail_mvd ? "FishVisuals: OK TrailRenderer.set_minVertexDistance"
                                  : "FishVisuals: MISS TrailRenderer.set_minVertexDistance");
    api->log(mod_id, fn_tr_set_euler ? "FishVisuals: OK Transform.set_localEulerAngles"
                                     : "FishVisuals: MISS Transform.set_localEulerAngles");
    api->log(mod_id, fn_set_sky ? "FishVisuals: OK RenderSettings.set_ambientSkyColor"
                                : "FishVisuals: MISS RenderSettings.set_ambientSkyColor");

    api->log(mod_id, "FishVisuals: STAGE 5 - ставлю хуки");
    bearite_hook_method(api, mod_id, "", "PlayerBearController", "AfterCharacterUpdate", 1,
                        (void*)hk_after_update, (void**)&orig_after_update);
    bearite_hook_method(api, mod_id, "UnityEngine", "Camera", "set_fieldOfView", 1,
                        (void*)hk_set_fov, (void**)&orig_set_fov);
    bearite_hook_method(api, mod_id, "UnityEngine", "Camera", "set_aspect", 1,
                        (void*)hk_set_aspect, (void**)&orig_set_aspect);
    api->log(mod_id, "FishVisuals: STAGE 6 - хуки стоят, init завершён");
    api->log(mod_id, (fn_tmp_settext && fn_tmp_setcolor) ? "FishVisuals: OK TMP_Text"
                                                         : "FishVisuals: MISS TMP_Text");
    bearite_hook_method(api, mod_id, "", "GameManager", "ShowBossName", 1,
                        (void*)hk_show_boss, (void**)&orig_show_boss);
    bearite_hook_method(api, mod_id, "", "ShickaTipsUI", "UpdateText", 0,
                        (void*)hk_shicka_update, (void**)&orig_shicka_update);
}
