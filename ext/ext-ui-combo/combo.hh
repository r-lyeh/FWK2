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
  // LICENSE
//
//   This software is dual-licensed to the public domain and under the following
//   license: you are granted a perpetual, irrevocable license to copy, modify,
//   publish, and distribute this file as you see fit.
//
// VERSION 
//   0.2.0  (2017-02-18)  Scored matches perform exhaustive search for best score
//   0.1.0  (2016-03-28)  Initial release
//
// AUTHOR
//   Forrest Smith
//
// NOTES
//   Compiling
//     You MUST add '#define FTS_FUZZY_MATCH_IMPLEMENTATION' before including this header in ONE source file to create implementation.
//
//   fuzzy_match_simple(...)
//     Returns true if each character in pattern is found sequentially within str
//
//   fuzzy_match(...)
//     Returns true if pattern is found AND calculates a score.
//     Performs exhaustive search via recursion to find all possible matches and match with highest score.
//     Scores values have no intrinsic meaning. Possible score range is not normalized and varies with pattern.
//     Recursion is limited internally (default=10) to prevent degenerate cases (pattern="aaaaaa" str="aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa")
//     Uses uint8_t for match indices. Therefore patterns are limited to 256 characters.
//     Score system should be tuned for YOUR use case. Words, sentences, file names, or method names all prefer different tuning.


#ifndef FTS_FUZZY_MATCH_H
#define FTS_FUZZY_MATCH_H


#include <cstdint> // uint8_t
#include <ctype.h> // ::tolower, ::toupper
#include <cstring> // memcpy

#include <cstdio>

// Public interface
namespace fts {
    static bool fuzzy_match_simple(char const * pattern, char const * str);
    static bool fuzzy_match(char const * pattern, char const * str, int & outScore);
    static bool fuzzy_match(char const * pattern, char const * str, int & outScore, uint8_t * matches, int maxMatches);
}


#ifdef FTS_FUZZY_MATCH_IMPLEMENTATION
namespace fts {

    // Forward declarations for "private" implementation
    namespace fuzzy_internal {
        static bool fuzzy_match_recursive(const char * pattern, const char * str, int & outScore, const char * strBegin,          
            uint8_t const * srcMatches,  uint8_t * newMatches,  int maxMatches, int nextMatch, 
            int & recursionCount, int recursionLimit);
    }

    // Public interface
    static bool fuzzy_match_simple(char const * pattern, char const * str) {
        while (*pattern != '\0' && *str != '\0')  {
            if (tolower(*pattern) == tolower(*str))
                ++pattern;
            ++str;
        }

        return *pattern == '\0' ? true : false;
    }

    static bool fuzzy_match(char const * pattern, char const * str, int & outScore) {
        
        uint8_t matches[256];
        return fuzzy_match(pattern, str, outScore, matches, sizeof(matches));
    }

    static bool fuzzy_match(char const * pattern, char const * str, int & outScore, uint8_t * matches, int maxMatches) {
        int recursionCount = 0;
        int recursionLimit = 10;

        return fuzzy_internal::fuzzy_match_recursive(pattern, str, outScore, str, nullptr, matches, maxMatches, 0, recursionCount, recursionLimit);
    }

    // Private implementation
    static bool fuzzy_internal::fuzzy_match_recursive(const char * pattern, const char * str, int & outScore, 
        const char * strBegin, uint8_t const * srcMatches, uint8_t * matches, int maxMatches, 
        int nextMatch, int & recursionCount, int recursionLimit)
    {
        // Count recursions
        ++recursionCount;
        if (recursionCount >= recursionLimit)
            return false;

        // Detect end of strings
        if (*pattern == '\0' || *str == '\0')
            return false;

        // Recursion params
        bool recursiveMatch = false;
        uint8_t bestRecursiveMatches[256];
        int bestRecursiveScore = 0;

        // Loop through pattern and str looking for a match
        bool first_match = true;
        while (*pattern != '\0' && *str != '\0') {
            
            // Found match
            if (tolower(*pattern) == tolower(*str)) {

                // Supplied matches buffer was too short
                if (nextMatch >= maxMatches)
                    return false;
                
                // "Copy-on-Write" srcMatches into matches
                if (first_match && srcMatches) {
                    memcpy(matches, srcMatches, nextMatch);
                    first_match = false;
                }

                // Recursive call that "skips" this match
                uint8_t recursiveMatches[256];
                int recursiveScore;
                if (fuzzy_match_recursive(pattern, str + 1, recursiveScore, strBegin, matches, recursiveMatches, sizeof(recursiveMatches), nextMatch, recursionCount, recursionLimit)) {
                    
                    // Pick best recursive score
                    if (!recursiveMatch || recursiveScore > bestRecursiveScore) {
                        memcpy(bestRecursiveMatches, recursiveMatches, 256);
                        bestRecursiveScore = recursiveScore;
                    }
                    recursiveMatch = true;
                }

                // Advance
                matches[nextMatch++] = (uint8_t)(str - strBegin);
                ++pattern;
            }
            ++str;
        }

        // Determine if full pattern was matched
        bool matched = *pattern == '\0' ? true : false;

        // Calculate score
        if (matched) {
            const int sequential_bonus = 15;            // bonus for adjacent matches
            const int separator_bonus = 30;             // bonus if match occurs after a separator
            const int camel_bonus = 30;                 // bonus if match is uppercase and prev is lower
            const int first_letter_bonus = 15;          // bonus if the first letter is matched

            const int leading_letter_penalty = -5;      // penalty applied for every letter in str before the first match
            const int max_leading_letter_penalty = -15; // maximum penalty for leading letters
            const int unmatched_letter_penalty = -1;    // penalty for every letter that doesn't matter

            // Iterate str to end
            while (*str != '\0')
                ++str;

            // Initialize score
            outScore = 100;

            // Apply leading letter penalty
            int penalty = leading_letter_penalty * matches[0];
            if (penalty < max_leading_letter_penalty)
                penalty = max_leading_letter_penalty;
            outScore += penalty;

            // Apply unmatched penalty
            int unmatched = (int)(str - strBegin) - nextMatch;
            outScore += unmatched_letter_penalty * unmatched;

            // Apply ordering bonuses
            for (int i = 0; i < nextMatch; ++i) {
                uint8_t currIdx = matches[i];

                if (i > 0) {
                    uint8_t prevIdx = matches[i - 1];

                    // Sequential
                    if (currIdx == (prevIdx + 1))
                        outScore += sequential_bonus;
                }

                // Check for bonuses based on neighbor character value
                if (currIdx > 0) {
                    // Camel case
                    char neighbor = strBegin[currIdx - 1];
                    char curr = strBegin[currIdx];
                    if (::islower(neighbor) && ::isupper(curr))
                        outScore += camel_bonus;

                    // Separator
                    bool neighborSeparator = neighbor == '_' || neighbor == ' ';
                    if (neighborSeparator)
                        outScore += separator_bonus;
                }
                else {
                    // First letter
                    outScore += first_letter_bonus;
                }
            }
        }

        // Return best result
        if (recursiveMatch && (!matched || bestRecursiveScore > outScore)) {
            // Recursive score is better than "this"
            memcpy(matches, bestRecursiveMatches, maxMatches);
            outScore = bestRecursiveScore;
            return true;
        }
        else if (matched) {
            // "this" score is better than recursive
            return true;
        }
        else {
            // no match
            return false;
        }
    }
} // namespace fts

#endif // FTS_FUZZY_MATCH_IMPLEMENTATION

#endif // FTS_FUZZY_MATCH_H


static const char* ICON_FA_SEARCH = UI_ICON(SEARCH);

namespace ImGui
{

static bool sortbysec_desc(const std::pair<int, int>& a, const std::pair<int, int>& b)
{
    return (b.second < a.second);
}

static int index_of_key(
    std::vector<std::pair<int, int> > pair_list,
    int key)
{
    for (int i = 0; i < pair_list.size(); ++i)
    {
        auto& p = pair_list[i];
        if (p.first == key)
        {
            return i;
        }
    }
    return -1;
}

// Copied from imgui_widgets.cpp
static float CalcMaxPopupHeightFromItemCount(int items_count)
{
    ImGuiContext& g = *GImGui;
    if (items_count <= 0)
        return FLT_MAX;
    return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
}

bool ComboWithFilter(const char* label, int* current_item, const std::vector<std::string>& items, int popup_max_height_in_items /*= -1 */)
{
    using namespace fts;

    ImGuiContext& g = *GImGui;

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const ImGuiStyle& style = g.Style;

    int items_count = static_cast<int>(items.size());

    // Use imgui Items_ getters to support more input formats.
    const char* preview_value = NULL;
    if (*current_item >= 0 && *current_item < items_count)
        preview_value = items[*current_item].c_str();

    static int focus_idx = -1;
    static char pattern_buffer[256] = { 0 };

    bool value_changed = false;

    const ImGuiID id = window->GetID(label);
    const ImGuiID popup_id = ImHashStr("##ComboPopup", 0, id); // copied from BeginCombo
    const bool is_already_open = IsPopupOpen(popup_id, ImGuiPopupFlags_None);
    const bool is_filtering = is_already_open && pattern_buffer[0] != '\0';

    int show_count = items_count;

    std::vector<std::pair<int, int> > itemScoreVector;
    if (is_filtering)
    {
        // Filter before opening to ensure we show the correct size window.
        // We won't get in here unless the popup is open.
        for (int i = 0; i < items_count; i++)
        {
            int score = 0;
            bool matched = fuzzy_match(pattern_buffer, items[i].c_str(), score);
            if (matched)
                itemScoreVector.push_back(std::make_pair(i, score));
        }
        std::sort(itemScoreVector.begin(), itemScoreVector.end(), sortbysec_desc);
        int current_score_idx = index_of_key(itemScoreVector, focus_idx);
        if (current_score_idx < 0 && !itemScoreVector.empty())
        {
            focus_idx = itemScoreVector[0].first;
        }
        show_count = static_cast<int>(itemScoreVector.size());
    }

    // Define the height to ensure our size calculation is valid.
    if (popup_max_height_in_items == -1) {
        popup_max_height_in_items = 5;
    }
    popup_max_height_in_items = ImMin(popup_max_height_in_items, show_count);


    if (!(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
    {
        int items = popup_max_height_in_items + 2; // extra for search bar
        SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(items)));
    }

    if (!BeginCombo(label, preview_value, ImGuiComboFlags_None))
        return false;


    if (!is_already_open)
    {
        focus_idx = *current_item;
        memset(pattern_buffer, 0, IM_ARRAYSIZE(pattern_buffer));
    }

    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(240, 240, 240, 255));
    ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(0, 0, 0, 255));
    ImGui::PushItemWidth(-FLT_MIN);
    // Filter input
    if (!is_already_open)
        ImGui::SetKeyboardFocusHere();
    InputText("##ComboWithFilter_inputText", pattern_buffer, 256, ImGuiInputTextFlags_AutoSelectAll);

    const ImVec2 label_size = CalcTextSize(ICON_FA_SEARCH, NULL, true);
    const ImVec2 search_icon_pos(
        ImGui::GetItemRectMax().x - label_size.x - style.ItemInnerSpacing.x * 2,
        window->DC.CursorPos.y + style.FramePadding.y + g.FontSize * 0.3f);
    RenderText(search_icon_pos, ICON_FA_SEARCH);

    ImGui::PopStyleColor(2);

    int move_delta = 0;
    if (IsKeyPressed(GetKeyIndex(ImGuiKey_UpArrow))
    ){
        --move_delta;
    }
    else if (IsKeyPressed(GetKeyIndex(ImGuiKey_DownArrow))
    ){
        ++move_delta;
    }
    else if (IsKeyPressed(GetKeyIndex(ImGuiKey_PageUp))
    ){
        move_delta -= popup_max_height_in_items;
    }
    else if (IsKeyPressed(GetKeyIndex(ImGuiKey_PageDown))
    ){
        move_delta += popup_max_height_in_items;
    }

    if (move_delta != 0)
    {
        if (is_filtering)
        {
            int current_score_idx = index_of_key(itemScoreVector, focus_idx);
            if (current_score_idx >= 0)
            {
                const int count = static_cast<int>(itemScoreVector.size());
                current_score_idx = ImClamp(current_score_idx + move_delta, 0, count - 1);
                focus_idx = itemScoreVector[current_score_idx].first;
            }
        }
        else
        {
            focus_idx = ImClamp(focus_idx + move_delta, 0, items_count - 1);
        }
    }

    // Copied from ListBoxHeader
    // If popup_max_height_in_items == -1, default height is maximum 7.
    float height_in_items_f = (popup_max_height_in_items < 0 ? ImMin(items_count, 7) : popup_max_height_in_items) + 0.25f;
    ImVec2 size;
    size.x = 0.0f;
    size.y = GetTextLineHeightWithSpacing() * height_in_items_f + g.Style.FramePadding.y * 2.0f;

    if (ImGui::BeginListBox("##ComboWithFilter_itemList", size))
    {
        for (int i = 0; i < show_count; i++)
        {
            int idx = is_filtering ? itemScoreVector[i].first : i;
            PushID((void*)(intptr_t)idx);
            const bool item_selected = (idx == focus_idx);
            const char* item_text = items[idx].c_str();
            if (Selectable(item_text, item_selected))
            {
                value_changed = true;
                *current_item = idx;
                CloseCurrentPopup();
            }

            if (item_selected)
            {
                SetItemDefaultFocus();
                // SetItemDefaultFocus doesn't work so also check IsWindowAppearing.
                if (move_delta != 0 || IsWindowAppearing())
                {
                    SetScrollHereY();
                }
            }
            PopID();
        }
        ImGui::EndListBox();

        if (IsKeyPressed(GetKeyIndex(ImGuiKey_Enter))
        ){
            value_changed = true;
            *current_item = focus_idx;
            CloseCurrentPopup();
        }
        else if (IsKeyPressed(GetKeyIndex(ImGuiKey_Escape))
        ){
            value_changed = false;
            CloseCurrentPopup();
        }
    }
    ImGui::PopItemWidth();
    ImGui::EndCombo();


    if (value_changed)
        MarkItemEdited(g.LastItemData.ID);

    return value_changed;
}

} // namespace ImGui



extern "C"
int igComboDemo(void) {
    static  const std::vector<std::string> data = {
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
    if (ImGui::ComboWithFilter("my combofilter", &current_item, data, -1)) {
        // selection occurred
    }
    ImGui::Text("Selection: %s, index = %d", """", current_item);
    return 0;
}