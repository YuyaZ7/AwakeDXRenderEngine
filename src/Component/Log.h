#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx12.h>
#include <imgui/imgui_impl_win32.h>
#include <vector>

struct Log {
	ImGuiTextBuffer Buf;
	ImGuiTextFilter Filter;
	ImVector<int> LineOffsets;// Index to lines offset
	bool AutoScroll;
};

// 向日志中添加一条信息
void AddLog(Log* log, const char* fmt, ...) IM_FMTARGS(2);

// 在 ImGui 窗口中显示日志
void ShowLog(Log* log);