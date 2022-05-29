#pragma once

class IRenderView {
public:
	get_virtual_fn(4, set_blend(float_t alpha), void(__thiscall*)(void*, float_t), alpha);
	get_virtual_fn(5, get_blend(), float_t(__thiscall*)(void*));
	get_virtual_fn(6, set_color_modulation(const float_t* color), void(__thiscall*)(void*, const float_t*), color);
	get_virtual_fn(7, get_color_modulation(float_t* color), void(__thiscall*)(void*, float_t*), color);
	get_virtual_fn(44, push_3d_view(IMatRenderContext* render_context, const CViewSetup& view, int32_t flags, ITexture* render_target, Frustum_t frustum_planes), void(__thiscall*)(void*, IMatRenderContext*, const CViewSetup&, int32_t, ITexture*, Frustum_t), render_context, view, flags, render_target, frustum_planes);
	get_virtual_fn(46, pop_view(IMatRenderContext* render_context, Frustum_t frustum_planes), void(__thiscall*)(void*, IMatRenderContext*, Frustum_t), render_context, frustum_planes);

};