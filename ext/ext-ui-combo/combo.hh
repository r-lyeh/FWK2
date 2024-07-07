#pragma once

// My modified ComboWithFilter with fts_fuzzy_match as include.
// Using dear imgui, v1.89 WIP
//
// Adds arrow/pgup/pgdn navigation, Enter to confirm, max_height_in_items, and
// fixed focus on open and avoids drawing past window edges.
// My contributions are CC0/public domain.

// Posted in issue: https://github.com/ocornut/imgui/issues/1658#issuecomment-1086193100

#include <vector>

// https://github.com/forrestthewoods/lib_fts/blob/632ca1ea82bdf65688241bb8788c77cb242fba4f/code/fts_fuzzy_match.h
#define FTS_FUZZY_MATCH_IMPLEMENTATION
#include "fts_fuzzy_match.hh"

namespace ImGui
{

bool ComboWithFilter(const char* label, int* current_item, int items_count, const char **items, int popup_max_height_in_items = -1)
{
    struct utils {
        static bool sortbysec_desc(const std::pair<int,int> &a, const std::pair<int,int> &b) {
            return b.second < a.second;
        }
        static int index_of_key(const std::vector<std::pair<int,int>> &pair_list, int key) {
            for( int i = 0; i < pair_list.size(); ++i ) {
                if( pair_list[i].first == key) {
                    return i;
                }
            }
            return -1;
        }
        static float CalcMaxPopupHeightFromItemCount(int items_count) { // Copied from imgui_widgets.cpp
            ImGuiContext& g = *GImGui;
            if( items_count > 0 )
                return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
            return FLT_MAX;
        }
    };

    #ifndef     ICON_FA_SEARCH
    const char* ICON_FA_SEARCH = UI_ICON(SEARCH);
    #endif

    static int focus_idx = -1;
    static char pattern_buffer[256] = { 0 };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    auto &io = ImGui::GetIO();
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    int show_count = items_count;

    // Use imgui Items_ getters to support more input formats.
    const char* preview_value = NULL;
    if (*current_item >= 0 && *current_item < items_count)
        preview_value = items[*current_item];

    const ImGuiID id = window->GetID(label);
    const ImGuiID popup_id = ImHashStr("##ComboPopup", 0, id); // copied from BeginCombo
    const bool is_open = IsPopupOpen(popup_id, ImGuiPopupFlags_None);
    const bool is_filtering = is_open && pattern_buffer[0] != '\0';

    std::vector<std::pair<int,int>> itemScoreVector;
    if (is_filtering)
    {
        // Filter before opening to ensure we show the correct size window.
        // We won't get in here unless the popup is open.
        for (int i = 0; i < items_count; i++) {
            int score = 0;
            if( fts::fuzzy_match(pattern_buffer, items[i], score) )
                itemScoreVector.push_back(std::make_pair(i, score));
        }
        std::sort(itemScoreVector.begin(), itemScoreVector.end(), utils::sortbysec_desc);
        int current_score_idx = utils::index_of_key(itemScoreVector, focus_idx);
        if (current_score_idx < 0 && !itemScoreVector.empty()) {
            focus_idx = itemScoreVector[0].first;
        }
        show_count = (int)(itemScoreVector.size());
    }

    // Define the height to ensure our size calculation is valid.
    if (popup_max_height_in_items == -1) {
        popup_max_height_in_items = 5;
    }
    popup_max_height_in_items = ImMin(popup_max_height_in_items, show_count);


    if (!(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint)) {
        int items = popup_max_height_in_items + 2; // extra for search bar
        SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, utils::CalcMaxPopupHeightFromItemCount(items)));
    }

    if (!BeginCombo(label, preview_value, ImGuiComboFlags_None))
        return false;

    bool is_changed = false;

    if( !is_open ) {
        focus_idx = *current_item;
        memset(pattern_buffer, 0, IM_ARRAYSIZE(pattern_buffer));
    }


    ImGui::PushItemWidth(-FLT_MIN);
    // Filter input
    if (!is_open)
        ImGui::SetKeyboardFocusHere(0);

    const int nav_keyboard_active = io.ConfigFlags & ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;

        //InputText("##ComboWithFilter_inputText", pattern_buffer, 256, ImGuiInputTextFlags_AutoSelectAll);
        InputTextWithHint("##ComboWithFilter_inputText", label, pattern_buffer, 256, ImGuiInputTextFlags_AutoSelectAll);

    io.ConfigFlags |= nav_keyboard_active;

    const ImVec2 label_size = CalcTextSize(ICON_FA_SEARCH, NULL, true);
    const ImVec2 search_icon_pos(
        ImGui::GetItemRectMax().x - label_size.x - style.ItemInnerSpacing.x * 2,
        window->DC.CursorPos.y + style.FramePadding.y + g.FontSize * 0.3f);
    RenderText(search_icon_pos, ICON_FA_SEARCH);

NavMoveRequestCancel();

    int move_delta = !!IsKeyPressed(GetKeyIndex(ImGuiKey_PageDown)) - !!IsKeyPressed(GetKeyIndex(ImGuiKey_PageUp));
    move_delta *= popup_max_height_in_items;
    move_delta += !!IsKeyPressed(GetKeyIndex(ImGuiKey_DownArrow)) - !!IsKeyPressed(GetKeyIndex(ImGuiKey_UpArrow));

    if (move_delta != 0) {
        if (is_filtering) {
            int current_score_idx = utils::index_of_key(itemScoreVector, focus_idx);
            if (current_score_idx >= 0) {
                const int count = (int)(itemScoreVector.size());
                current_score_idx = ImClamp(current_score_idx + move_delta, 0, count - 1);
                focus_idx = itemScoreVector[current_score_idx].first;
            }
        }
        else {
            focus_idx = ImClamp(focus_idx + move_delta, 0, items_count - 1);
        }
    }

    // Copied from ListBoxHeader
    // If popup_max_height_in_items == -1, default height is maximum 7.
    float height_in_items_f = (popup_max_height_in_items < 0 ? ImMin(items_count, 7) : popup_max_height_in_items) + 0.25f;
    ImVec2 size = ImVec2(0, GetTextLineHeightWithSpacing() * height_in_items_f + g.Style.FramePadding.y * 2);

    if (ImGui::BeginListBox("##ComboWithFilter_itemList", size))
    {
        for (int i = 0; i < show_count; i++)
        {
            int idx = is_filtering ? itemScoreVector[i].first : i;
            PushID((void*)(intptr_t)idx);
            const bool item_selected = (idx == focus_idx);
            const char* item_text = items[idx];

            if( Selectable(item_text, item_selected, ImGuiSelectableFlags_SelectOnNav) ) {
                is_changed = true;
                *current_item = idx;
                CloseCurrentPopup();
            }

            if (item_selected) {
                SetItemDefaultFocus();
                if (move_delta != 0 || IsWindowAppearing()) // SetItemDefaultFocus doesn't work so also check IsWindowAppearing.
                    SetScrollHereY();
            }

            PopID();
        }
        ImGui::EndListBox();

        if (IsKeyPressed(GetKeyIndex(ImGuiKey_Escape))) {
            is_changed = false;
            CloseCurrentPopup();
        }
        else if (IsKeyPressed(GetKeyIndex(ImGuiKey_Enter)) || IsKeyPressed(GetKeyIndex(ImGuiKey_Tab)) || IsKeyPressed(GetKeyIndex(ImGuiKey_RightArrow))) { 
            is_changed = true;
            *current_item = focus_idx;
            CloseCurrentPopup();
        }
    }
    ImGui::PopItemWidth();
    ImGui::EndCombo();

    if (is_changed)
        MarkItemEdited(g.LastItemData.ID);

    return is_changed;
}

} // namespace ImGui



extern "C"
int igComboDemo(void) {
    static const char *data[] = {
        "",
        "AnimGraphNode_CopyBone",
        "ce skipaa",
        "ce skipscreen",
        "ce skipsplash",
        "ce skipsplashscreen",
        "client_unit.cpp",
        "letrograd",
        "level",
        "leveler",
        "MacroCallback.cpp",
        "Miskatonic university",
        "MockAI.h",
        "MockGameplayTasks.h",
        "MovieSceneColorTrack.cpp",
        "r.maxfps",
        "r.maxsteadyfps",
        "reboot",
        "rescale",
        "reset",
        "resource",
        "restart",
        "retrocomputer",
        "retrograd",
        "return",
        "slomo 10",
        "SVisualLoggerLogsList.h",
        "The Black Knight",
    }; 
    static int current_item = 0;
    if (ImGui::ComboWithFilter("my combofilter", &current_item, countof(data), data, -1)) {
        // selection occurred
    }
    ImGui::Text("Selection: %s, index = %d", data[current_item], current_item);
    return 0;
}
