void postcook() {
//#ifdef ENABLE_LEGAL_SCREEN
//#include ENABLE_LEGAL_SCREEN
//#endif

#if 0 // ENABLE_LEGAL_SCREEN
    // background image+logo+headline+text
    texture_t logo_bg = texture(LEGAL_SCREEN_BACKGROUND_IMG, 0);
    if (logo_bg.id != texture_checker().id) {
        texture_t logo_mask = texture(LEGAL_SCREEN_LOGO_IMG, 0);
        font_face(FONT_FACE3, LEGAL_SCREEN_FONT_TTF, LEGAL_SCREEN_FONT_SIZE, FONT_EU | FONT_4096 | FONT_OVERSAMPLE_X | FONT_OVERSAMPLE_Y);
        sprite(logo_bg, vec3(w/2.f, h/2.f, 0).array, 0.0f, 0xFFFFFFFF, SPRITE_CENTERED|SPRITE_RESOLUTION_INDEPENDANT);
        sprite(logo_mask, vec3(w/2.f, h/2.f, 1).array, 0.0f, 0xFFFFFFFF, SPRITE_CENTERED|SPRITE_RESOLUTION_INDEPENDANT);
        sprite_flush();
        font_print(
            FONT_BOTTOM FONT_CENTER FONT_FACE3 FONT_H3
            LEGAL_SCREEN_HEADLINE "\n\n\n\n\n\n\n\n\n\n"
        );

        char *legalese = va(FONT_CENTER FONT_FACE3 FONT_H4 LEGAL_SCREEN_TEXT);
        vec2 dims = font_rect(legalese);
        font_goto(0, window_height()-dims.y);
        font_print(legalese);

        glfwSwapBuffers(window);
    }
#endif
}

