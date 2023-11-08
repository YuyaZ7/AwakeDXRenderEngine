#include "Log.h"
// 向日志中添加一条信息
void AddLog(Log* log, const char* fmt, ...) IM_FMTARGS(2) {
	int old_size = log->Buf.size();
	va_list args;
	va_start(args, fmt);
	log->Buf.appendfv(fmt, args);
	va_end(args);
	for (int new_size = log->Buf.size(); old_size < new_size; old_size++)
		if (log->Buf[old_size] == '\n')
			log->LineOffsets.push_back(old_size);
}

// 在 ImGui 窗口中显示日志
void ShowLog(Log* log) {
	ImGui::Begin("Log");

	// 处理自动滚动
	if (ImGui::Button("Scroll to bottom"))
		log->AutoScroll = true;
	ImGui::SameLine();
	bool copy = ImGui::Button("Copy");
	ImGui::SameLine();
	log->Filter.Draw("Filter", -100.0f);

	// 显示日志内容
	ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
	if (copy)
		ImGui::LogToClipboard();

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	const char* buf_begin = log->Buf.begin();
	const char* line = buf_begin;
	for (int line_no = 0; line != NULL; line_no++) {
		const char* line_end = (line_no < log->LineOffsets.Size) ? buf_begin + log->LineOffsets[line_no] : NULL;
		if (log->Filter.PassFilter(line, line_end))
			ImGui::TextUnformatted(line, line_end);
		line = line_end && line_end[1] ? line_end + 1 : NULL;
	}
	if (log->AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);
	ImGui::PopStyleVar();
	ImGui::EndChild();
	ImGui::End();
}