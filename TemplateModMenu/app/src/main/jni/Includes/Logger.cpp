#include "Logger.h"

#include "Logger.h"
#include "imgui/imgui.h"
#include <cstdio>
#include <string.h>

namespace logger
{
    ImGuiTextBuffer Buf;
    ImGuiTextFilter Filter;
    ImVector<int> LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
    bool AutoScroll = true;    // Keep scrolling if already at the bottom.
    bool WordWrap = true;

    void Clear()
    {
        Buf.clear();
        LineOffsets.clear();
        LineOffsets.push_back(0);
    }

    // void AddLog(const char *fmt, ...)
    // {
    //     int old_size = Buf.size();
    //     va_list args;
    //     va_start(args, fmt);
    //     Buf.appendfv(fmt, args);
    //     va_end(args);
    //     for (int new_size = Buf.size(); old_size < new_size; old_size++)
    //         if (Buf[old_size] == '\n')
    //             LineOffsets.push_back(old_size + 1);
    // }

    void AddLog(const char *prefix, const char *fmt, ...)
    {
        int old_size = Buf.size();
        int modifiedFmtLength = strlen(prefix) + strlen(fmt);
        char *modifiedBuf = new char[modifiedFmtLength + 1];

        sprintf(modifiedBuf, "%s%s\n", prefix, fmt);

        va_list args;
        va_start(args, fmt);
        Buf.appendfv(modifiedBuf, args);
        va_end(args);

        delete[] modifiedBuf;
        for (int new_size = Buf.size(); old_size < new_size; old_size++)
            if (Buf[old_size] == '\n')
                LineOffsets.push_back(old_size + 1);
    }

    // NEVER CALL LOG HERE
    void Draw(const char *title, bool *p_open)
    {
        // if (!ImGui::Begin(title, p_open))
        //{
        //     ImGui::End();
        //     return;
        // }

        // Options menu
        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &AutoScroll);
            ImGui::Checkbox("Word-Wrap", &WordWrap);
            ImGui::EndPopup();
        }

        // Main window
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");

        ImGui::SameLine();
        bool clear = ImGui::Button("Clear##log");
        ImGui::SameLine();
        ImGui::SameLine();
        // Filter.Draw("Filter", -100.0f);

        ImGui::Separator();

        static float fontScale = 0.5f;
        ImGui::SliderFloat("Scale", &fontScale, 0.1f, 1.f, "%.2f");
        static auto font = ImGui::GetFont();
        static auto origScale = font->Scale;
        font->Scale = fontScale;
        ImGui::PushFont(font);
        if (ImGui::BeginChild("scrolling", ImVec2(0, 0), false, WordWrap ? 0 : ImGuiWindowFlags_HorizontalScrollbar))
        {
            if (clear)
                Clear();

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            const char *buf = Buf.begin();
            const char *buf_end = Buf.end();

            static auto Text = [](const char *line_start, const char *line_end)
            {
                if (Filter.PassFilter(line_start, line_end))
                {
                    ImVec4 color{1.f, 0.2f, 1.f, 1.0f};
                    if (strncmp(line_start, "[D]", strlen("[D]")) == 0)
                        color = {1.f, 1.f, 0.5, 1.f};
                    else if (strncmp(line_start, "[E]", strlen("[E]")) == 0)
                        color = {1.f, 0.5f, 0.5f, 1.f};
                    else if (strncmp(line_start, "[I]", strlen("[I]")) == 0)
                        color = {0.f, 1.f, 1.f, 1.f};
                    if (WordWrap)
                        ImGui::PushTextWrapPos();
                    ImGui::PushStyleColor(0, color);
                    ImGui::TextUnformatted(line_start + strlen("[D] "), line_end);
                    ImGui::PopStyleColor();
                    if (WordWrap)
                        ImGui::PopTextWrapPos();
                }
            };

            if (Filter.IsActive())
            {
                // In this example we don't use the clipper when Filter is enabled.
                // This is because we don't have random access to the result of our filter.
                // A real application processing logs with ten of thousands of entries may want to store the result of
                // search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
                for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
                {
                    const char *line_start = buf + LineOffsets[line_no];
                    const char *line_end =
                        (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                    Text(line_start, line_end);
                }
            }
            else
            {
                // The simplest and easy way to display the entire buffer:
                //   ImGui::TextUnformatted(buf_begin, buf_end);
                // And it'll just work. TextUnformatted() has specialization for large blob of text and will
                // fast-forward to skip non-visible lines. Here we instead demonstrate using the clipper to only process
                // lines that are within the visible area. If you have tens of thousands of items and their processing
                // cost is non-negligible, coarse clipping them on your side is recommended. Using ImGuiListClipper
                // requires
                // - A) random access into your data
                // - B) items all being the  same height,
                // both of which we can handle since we have an array pointing to the beginning of each line of text.
                // When using the filter (in the block of code above) we don't have random access into the data to
                // display anymore, which is why we don't use the clipper. Storing or skimming through the search result
                // would make it possible (and would be recommended if you want to search through tens of thousands of
                // entries).
                ImGuiListClipper clipper;
                clipper.Begin(LineOffsets.Size);
                while (clipper.Step())
                {
                    for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                    {
                        const char *line_start = buf + LineOffsets[line_no];
                        const char *line_end =
                            (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                        Text(line_start, line_end);
                    }
                }
                clipper.End();
            }
            ImGui::PopStyleVar();

            // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the
            // frame. Using a scrollbar or mouse-wheel will take away from the bottom edge.
            if (AutoScroll && ImGui::GetScrollY() + 10.f >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);
        }
        font->Scale = origScale;
        ImGui::EndChild();
        ImGui::PopFont();
        // ImGui::End();
    }
}; // namespace logger
