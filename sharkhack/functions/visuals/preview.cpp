#include "preview.h"
#include "../../utils/math.h"

void preview::on_init() {
    if (!preview_texture) {
        g_mat_system->BeginRenderTargetAllocation();
        preview_texture = g_mat_system->CreateNamedRenderTargetTextureEx("esp_preview", 360, 480, RenderTargetSizeMode_t::RT_SIZE_NO_CHANGE, g_mat_system->GetBackBufferFormat(), MATERIAL_RT_DEPTH_SHARED, 0x4 | 0x8, 0x1);
        g_mat_system->finish_render_target_allocation();
    }

    if (!preview_model) {
        CMergedMdl* new_model = static_cast<CMergedMdl*>(g_mem_alloc->Alloc(0x75C));
        reinterpret_cast<void(__thiscall*)(void*)>(memory.create_model)(new_model);

        preview_model = new_model;

        preview_model->set_mdl("models/player/custom_player/uiplayer/animset_uiplayer.mdl");
        preview_model->set_merge_mdl("models/player/custom_player/legacy/tm_balkan_variantg.mdl" );
        preview_model->set_sequence(214 , false);
        preview_model->setup_bones_for_attachment_queries();
        preview_model->set_merge_mdl("models/weapons/w_rif_ak47.mdl");
    }

    preview_model->root_mdl.time = g_globals->curtime;

    auto render_context = g_mat_system->get_render_context();

    CViewSetup view_setup;
    memset(&view_setup, 0, sizeof(view_setup));
    view_setup.x = 0;
    view_setup.y = 0;
    view_setup.width = 360;
    view_setup.height = 480;
    view_setup.fov = 9.96f;
    view_setup.origin = Vector(-500.0f, -0.83f, 48.8f);
    view_setup.angles = Vector(0.0f, 0.0f, 0.0f);
    view_setup.m_near = 7.0f;
    view_setup.m_far = 1000.0f;

    render_context->push_render_target_and_viewport();
    render_context->set_render_target(preview_texture);
    render_context->clear_color_4ub(0, 0, 0, 0);
    render_context->clear_buffers(true, true, true);

    render_context->set_lighting_origin(-115.0f, 0.0f, 40.0f);
    render_context->set_int_rendering_parameter(10, 0);

    static Vector4D white[6] = { { 0.4f, 0.4f, 0.4f, 1.0f }, { 0.4f, 0.4f, 0.4f, 1.0f }, { 0.4f, 0.4f, 0.4f, 1.0f }, { 0.4f, 0.4f, 0.4f, 1.0f }, { 0.4f, 0.4f, 0.4f, 1.0f }, { 0.4f, 0.4f, 0.4f, 1.0f } };
    g_studio_render->set_ambient_light_colors(white);
    g_studio_render->set_local_lights(0, nullptr);

    Frustum_t dummy_frustum;
    g_render_view->push_3d_view(render_context, view_setup, 0, preview_texture, dummy_frustum);
    g_model_render->suppress_engine_lighting(true);

    float_t color[3] = { 1.0f, 1.0f, 1.0f };
    g_render_view->set_color_modulation(color);
    g_render_view->set_blend(1.0f);

    matrix3x4_t mat_player_view;
    math::angle_matrix({ 0.0f, preview_matrix, 0.0f }, mat_player_view, { 0.0f, 0.0f, 0.0f });

    draw_model = true;
    preview_model->draw(mat_player_view);
    draw_model = false;

    g_model_render->suppress_engine_lighting(false);
    g_render_view->pop_view(render_context, dummy_frustum);

    render_context->bind_local_cubemap(nullptr);
    render_context->pop_render_target_and_viewport();
}