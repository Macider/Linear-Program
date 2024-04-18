#pragma once

#include <easyx.h>
#include <functional>
#include <string>
#include <vector>
#include <iostream>
#include <cassert>

using namespace std;

// 定义Button类，表示一个按钮
class Button {
private:
	int x; // 按钮左上角x坐标
	int y; // 按钮左上角y坐标
	int width; // 按钮宽度
	int height; // 按钮高度
	float scale; // 缩放比例，用于实现鼠标悬停效果
	bool isMouseOver; // 表示鼠标是否在按钮上方
	string text; // 按钮文本
	function<void()> onClick; // 点击按钮触发的函数，要求无参数并且无返回值

public:

	Button(int x, int y, int width, int height, const string& text, const function<void()>& onClick)
		: x(x), y(y), width(width), height(height), text(text), onClick(onClick), scale(1.0f), isMouseOver(false) {}

	void setXY(int x_, int y_) {
		x = x_;
		y = y_;
	}
	void setWH(int w_, int h_) {
		width = w_;
		height = h_;
	}
	void setText(const string str) {
		text = str;
	}
	string getText() {
		return text;
	}
	void click() {
		onClick();
	}
	void setFunc(const function<void()> func) {
		onClick = func;
	}

	// 检查鼠标是否在按钮上方
	void checkMouseOver(int mouseX, int mouseY) {
		isMouseOver = (mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height);
		if (isMouseOver)
			scale = 0.9f; // 鼠标悬停时缩放按钮
		else
			scale = 1.0f; // 恢复按钮原始大小
	}

	// 检查鼠标点击是否在按钮内，并执行函数
	bool checkClick(int mouseX, int mouseY) {
		if (mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height) {
			onClick(); // 执行按钮点击时的函数
			isMouseOver = false;
			scale = 1.0f;
			return true;
		}
		return false;
	}

	// 绘制按钮
	void draw() {
		int scaledWidth = width * scale; // 缩放后的按钮宽度
		int scaledHeight = height * scale; // 缩放后的按钮高度
		int scaledX = x + (width - scaledWidth) / 2; // 缩放后的按钮x坐标
		int scaledY = y + (height - scaledHeight) / 2; // 缩放后的按钮y坐标

		if (isMouseOver) {
			setlinecolor(RGB(0, 120, 215)); // 鼠标悬停时按钮边框颜色
			setfillcolor(RGB(229, 241, 251)); // 鼠标悬停时按钮填充颜色
		}
		else {
			setlinecolor(RGB(173, 173, 173)); // 按钮边框颜色
			setfillcolor(RGB(225, 225, 225)); // 按钮填充颜色
		}

		fillrectangle(scaledX, scaledY, scaledX + scaledWidth, scaledY + scaledHeight); // 绘制按钮
		settextcolor(BLACK); // 设置文本颜色为黑色
		setbkmode(TRANSPARENT); // 设置文本背景透明
		settextstyle(20 * scale, 0, "宋体"); // 设置文本大小和字体
		//居中显示按钮文本
		int textX = scaledX + (scaledWidth - textwidth(text.c_str())) / 2; // 计算文本在按钮中央的x坐标
		int textY = scaledY + (scaledHeight - textheight("宋体")) / 2; // 计算文本在按钮中央的y坐标
		outtextxy(textX, textY, text.c_str()); // 在按钮上绘制文本
	}
};

// 定义TextBox类，表示一个文本框
class TextBox {
private:
	int x;
	int y;
	int width;
	int height;
	int maxWord;    //最长单词个数
	string text;   //文本
	bool isSelected;    //是否被选中
	bool showCursor;    //是否显示光标
	int cursorPos;      //光标位置

public:
	TextBox(int x, int y, int width, int height, int maxWord)
		: x(x), y(y), width(width), height(height), maxWord(maxWord), isSelected(false), showCursor(false), cursorPos(0) {}

	const string& getText() const { return text; }		// 返回储存的文本内容

	bool getSelect() const { return isSelected; }		// 返回是否被选中

	bool isEmpty() const { return text.empty(); }		// 文本框是否为空

	void clear() { text = ""; }							// 清楚文本框内容

	void setText(const string& newText) {
		text = newText;
		cursorPos = text.length();
	}

	// 需要有修改xy的接口
	void setXY(int x_, int y_) {
		x = x_;
		y = y_;
	}
	void setWH(int w_, int h_) {
		width = w_;
		height = h_;
	}

	void draw(COLORREF colorFilled = WHITE, COLORREF colorSelected = YELLOW) {
		//setfillcolor(WHITE);
		//if (isSelected)
		//	setlinecolor(RGB(0, 120, 215));
		//else
		//	setlinecolor(RGB(122, 122, 122));
		setlinecolor(BLACK);
		if (isSelected)
			setfillcolor(colorSelected);
		else
			setfillcolor(colorFilled);
		fillrectangle(x, y, x + width, y + height);		// 文本框外框

		settextcolor(BLACK);
		setbkmode(TRANSPARENT);
		settextstyle(height * 3 / 4, 0, "宋体");

		outtextxy(x + 5, y + (height - textheight("宋体")) / 2, text.c_str());

		setlinecolor(BLACK);
		if (isSelected && showCursor) {					// 绘制光标
			int cursorX = x + 5 + textwidth(text.substr(0, cursorPos).c_str());
			line(cursorX, y + 2 + height / 8, cursorX, y + height * 7 / 8 - 2);
		}
	}

	bool checkClick(int mouseX, int mouseY) {
		if (mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height) {
			isSelected = true;
			return true;
		}
		else {
			isSelected = false;
			cursorPos = text.length();
			return false;
		}
	}

	void keyInput(char ch) {
		if (isSelected) {
			switch (ch) {
			case '\b':				// 用户按退格键，删掉一个字符
				if (!text.empty() && cursorPos > 0) {
					text.erase(cursorPos - 1, 1);
					cursorPos--;
				}
				break;
			case '\r':
			case '\n':
				cursorPos = text.length();
				isSelected = false;
				break;
			default:				// 用户按其它键，接受文本输入
				if (text.length() < maxWord) {
					text.insert(cursorPos, 1, ch);
					cursorPos++;
				}
			}
		}
	}

	void updateCursor() {
		static DWORD lastTick = GetTickCount();
		DWORD currentTick = GetTickCount();
		if (currentTick - lastTick >= 500) { // 每500毫秒切换光标状态
			showCursor = !showCursor;
			lastTick = currentTick;
		}
	}
};

// 定义TableBox类，表示一个由文本框及按钮组成的表格
class TableBox {
private:
	int x, y;								// 表格左上角坐标
	int width, height;						// 表格宽度、高度
	int rowHeight, colWidth;				// 行高、列宽
	int visibleRowCount, visibleColCount;	// 最多可见行、列数
	int selectedRow, selectedCol;			// 被选中的行、列
	int rows, cols;							// 总行、列数
	vector<vector<TextBox>> cells;			// 按列储存
	typedef pair<Button, pair<int, int>> ButtonInform;
	vector<ButtonInform> buttonsInform;		// 部分格子可以改为按钮
	// 竖向滑动条相关
	int scrollRowOffset;
	int scrollbarWidth;		// 竖向滑动条宽度
	int handleHeight;		// 竖向滑块高度
	int handleY;			// 竖向滑块所在位置
	// 横向滑动条相关
	int scrollColOffset;
	int scrollbarHeight;	// 横向滑动条高度
	int handleWidth;		// 横向滑块宽度
	int handleX;			// 横向滑块所在位置

public:
	TableBox(int x, int y, int width, int height, int visibleColCount, int visibleRowCount)
		: x(x), y(y), width(width), height(height), visibleColCount(visibleColCount), visibleRowCount(visibleRowCount) {
		rows = 25, cols = 25;
		int rowCount = min(visibleRowCount, rows);
		int colCount = min(visibleColCount, cols);

		rowHeight = height / rowCount;
		colWidth = width / colCount;


		selectedRow = -1;
		scrollRowOffset = 0;
		scrollbarWidth = 20;
		handleHeight = 30;
		handleY = 0;

		selectedCol = -1;
		scrollColOffset = 0;
		scrollbarHeight = 20;
		handleWidth = 30;
		handleX = 0;

		for (int j = 0; j < cols; ++j) {
			vector<TextBox> col;
			for (int i = 0; i < rows; ++i)
				col.push_back(TextBox(x + j * colWidth, y + i * rowHeight, colWidth, rowHeight, 100));
			cells.push_back(col);
		}
		buttonsInform.clear();
	}

	// 重新计算各个格子的位置及大小
	void calibrateTable(){
		for (int j = 0; j < cols; j++) {
			int colX = x + (j - scrollColOffset) * colWidth;
			for (int i = 0; i < rows; i++) {
				int rowY = y + (i - scrollRowOffset) * rowHeight;
				bool isButton = false;
				for (ButtonInform& buttonInform : buttonsInform) {
					if (buttonInform.second != make_pair(j, i))
						continue;
					isButton = true;
					buttonInform.first.setXY(colX, rowY);
					buttonInform.first.setWH(colWidth, rowHeight);
					break;
				}
				if (isButton)
					continue;
				cells.at(j).at(i).setXY(colX, rowY);
				cells.at(j).at(i).setWH(colWidth, rowHeight);
			}
		}
	}

	int getSelectedCol() const { return selectedCol; }
	int getSelectedRow() const { return selectedRow; }
	int getCols() const { return cols; }
	int getRows() const { return rows; }

	void addCol(int index = -1) {	// 在[index]的前面插入列，默认在最后
		assert(-1 <= index && index <= cols);
		cols++;
		int colCount = min(visibleColCount, cols);
		colWidth = width / colCount;
		vector<TextBox> col;
		if (index == -1 || index == cols - 1) {
			for (int i = 0; i < rows; ++i)
				col.push_back(TextBox(x + (cols - 1) * colWidth, y + i * rowHeight, colWidth, rowHeight, 100));
			cells.push_back(col);	// 新push的col的下标为cols-1

		}
		else {				// index in [0,cols-2]
			for (int i = 0; i < rows; ++i)
				col.push_back(TextBox(x + index * colWidth, y + i * rowHeight, colWidth, rowHeight, 100));
			cells.emplace(cells.begin() + index, col);	//新插入的col下标为index
			// 下标>=index的button在表格中的相对位置会改变
			for (ButtonInform& buttonInform : buttonsInform)
				if (buttonInform.second.first>=index)
					buttonInform.second.first++;
		
		}
		// 表格数变化时进行校准
		calibrateTable();
	}
	void addRow(int index = -1) {	// 在[index]的前面插入行，默认在最后
		assert(-1 <= index && index <= rows);
		rows++;
		int rowCount = min(visibleRowCount, rows);
		rowHeight = height / rowCount;
		if (index == -1 || index == rows - 1)
			for (int j = 0; j < cols; ++j)
				cells.at(j).push_back(TextBox(x + j * colWidth, y + (rows - 1) * rowHeight, colWidth, rowHeight, 100));
		else { 				// index in [0,cols-2]
			for (int j = 0; j < cols; ++j)
				cells.at(j).emplace(cells.at(j).begin() + index, TextBox(x + j * colWidth, y + index * rowHeight, colWidth, rowHeight, 100));
			// 下标>=index的button在表格中的相对位置会改变
			/*for (int j = 0; j < cols; j++) 
				for (int i = index; i < rows; i++) 
					for (ButtonInform& buttonInform : buttonsInform) {
						if (buttonInform.second != make_pair(j, i))
							continue;
						buttonInform.second.second++;
						break;
					}*/
			for (ButtonInform& buttonInform : buttonsInform)
				if (buttonInform.second.second >= index)
					buttonInform.second.second++;
				
		}
		// 表格数变化时进行校准
		calibrateTable();
	}

	void deletaCol(int index = -1) {	// 删除[index%cols]
	}
	void deleteRow(int index = -1) {	// 删除[index%rows]
	}

	// 将某个单元格设置为按钮
	void toButton(Button button, int col, int row) {
		pair<int, int> colAndRow = make_pair(col, row);
		button.setWH(colWidth, rowHeight);
		button.setXY(x + col * colWidth, y + row * rowHeight);
		buttonsInform.push_back(make_pair(button, colAndRow));
	}
	// 检查某个单元格是否为按钮
	bool isButton(int col, int row) {
		for (ButtonInform& buttonInform : buttonsInform)
			if (buttonInform.second == make_pair(col, row))
				return true;
		return false;
	}
	Button* getButton(int col, int row) {
		for (ButtonInform& buttonInform : buttonsInform)
			if (buttonInform.second == make_pair(col, row))
				return &buttonInform.first;
		return NULL;
	}

	// 竖向滑动条上滑
	void scrollUp() {
		if (scrollRowOffset <= 0) {
			scrollRowOffset = 0;
			return;
		}
		scrollRowOffset--;
		// 滚动条变化时进行校准
		calibrateTable();
	}
	// 竖向滑动条下滑
	void scrollDown() {
		int maxScrollOffset = max(rows - visibleRowCount, 0);
		if (scrollRowOffset >= maxScrollOffset) {
			scrollRowOffset = maxScrollOffset;
			return;
		}
		scrollRowOffset++;
		// 滚动条变化时进行校准
		calibrateTable();
	}
	// 横向滑动条左滑
	void scrollLeft() {
		if (scrollColOffset <= 0) {
			scrollColOffset = 0;
			return;
		}
		scrollColOffset--;
		// 滚动条变化时进行校准
		calibrateTable();
	}
	// 横向滑动条右滑
	void scrollRight() {
		int maxScrollOffset = max(cols - visibleColCount, 0);
		if (scrollColOffset >= maxScrollOffset) {
			scrollColOffset = maxScrollOffset;
			return;
		}
		scrollColOffset++;
		// 滚动条变化时进行校准
		calibrateTable();
	}
	// 处理滑动条
	void scroll(int mouseX, int mouseY, int wheel) {
		// 纵向滑动条
		bool changeOrNot = false;
		if (mouseX >= x + width && mouseX <= x + width + scrollbarWidth && mouseY >= y && mouseY <= y + height) {
			changeOrNot = true;
			if (wheel > 0)
				scrollUp();
			else if (wheel < 0)
				scrollDown();
		}
		// 横向滑动条
		else if (mouseX >= x && mouseX <= x + width && mouseY >= y + height && mouseY <= y + height + scrollbarHeight) {
			changeOrNot = true;
			if (wheel > 0)
				scrollLeft();
			else if (wheel < 0)
				scrollRight();
		}
	}

	// 处理鼠标移动
	void checkMouseOver(int mouseX, int mouseY) {
		for (ButtonInform& buttonInform : buttonsInform)
			buttonInform.first.checkMouseOver(mouseX, mouseY);
		if (mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height) {
			int dataColIndex = (mouseX - x) / colWidth + scrollColOffset;
			int dataRowIndex = (mouseY - y) / rowHeight + scrollRowOffset;
			// 似乎并无作用
		}
	}

	// 处理鼠标点击
	void handleMouseClick(int mouseX, int mouseY) {
		for (int j = 0; j < cols; ++j)
			for (int i = 0; i < rows; ++i)
				if (!isButton(j, i))
					cells.at(j).at(i).checkClick(mouseX, mouseY);
		if (mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height) {
			int dataRowIndex = (mouseY - y) / rowHeight + scrollRowOffset;
			int dataColIndex = (mouseX - x) / colWidth + scrollColOffset;
			for (ButtonInform& buttonInform : buttonsInform)
				if (buttonInform.second == make_pair(dataColIndex, dataRowIndex)) {
					buttonInform.first.checkClick(mouseX, mouseY);
					return;
				}
			if (dataRowIndex >= 0 && dataRowIndex < rows)
				selectedRow = dataRowIndex;
			else
				selectedRow = -1;
			if (dataColIndex >= 0 && dataColIndex < cols)
				selectedCol = dataColIndex;
			else
				selectedCol = -1;
		}
	}

	// 处理键盘输入
	void keyInput(char ch) {
		if (selectedCol == -1 || selectedRow == -1)
			return;
		assert(cells.at(selectedCol).at(selectedRow).getSelect());
		cells.at(selectedCol).at(selectedRow).keyInput(ch);
	}

	void draw() {
		setbkmode(TRANSPARENT);		// 设置文字背景为透明
		// 绘制表格区域
		setfillcolor(WHITE);
		solidrectangle(x, y, x + width, y + height);
		setlinecolor(BLACK);
		settextstyle(12, 0, "宋体");

		// 绘制滚动条背景
		int scrollbarX = x + width;		// 表格右边界
		int scrollbarY = y + height;	// 表格下边界
		setfillcolor(LIGHTGRAY);
		solidrectangle(scrollbarX, y, scrollbarX + scrollbarWidth, scrollbarY);
		solidrectangle(x, scrollbarY, scrollbarX, scrollbarY + scrollbarHeight);

		int handlePosX, handlePosY;
		setfillcolor(DARKGRAY);
		// 计算位置大小并绘制竖向滑块
		handlePosX = scrollbarX;
		int maxHandleY = height - handleHeight;		// 实际能移动的距离
		handlePosY = y + maxHandleY * double(scrollRowOffset) / max(rows - visibleRowCount, 1);
		solidrectangle(handlePosX, handlePosY, handlePosX + scrollbarWidth, handlePosY + handleHeight);
		// 计算位置大小并绘制横向滑块
		handlePosY = scrollbarY;
		int maxHandleX = width - handleWidth;		// 实际能移动的距离
		handlePosX = x + maxHandleX * double(scrollColOffset) / max(cols - visibleColCount, 1);
		solidrectangle(handlePosX, handlePosY, handlePosX + handleWidth, handlePosY + scrollbarHeight);



		// 计算需要绘制的行、列数
		int rowCount = min(visibleRowCount, rows);
		int colCount = min(visibleColCount, cols);

		// 绘制表中固定部分


		// 绘制表格主体内容
		for (int j = 0; j < colCount; ++j) {
			int colX = x + j * colWidth;
			int dataColIndex = j + scrollColOffset;
			bool isSelectedCol = (dataColIndex == selectedCol);
			for (int i = 0; i < rowCount; ++i) {
				int rowY = y + i * rowHeight;
				int dataRowIndex = i + scrollRowOffset;
				bool isSelectedRow = (dataRowIndex == selectedRow);

				bool isButton = false;
				for (ButtonInform& buttonInform : buttonsInform) {		// 默认创建副本，引用需要加&
					if (buttonInform.second != make_pair(dataColIndex, dataRowIndex))
						continue;
					isButton = true;

					buttonInform.first.draw();
					break;
				}
				if (isButton)
					continue;
				//cells.at(dataColIndex).at(dataRowIndex).setXY(colX, rowY);
				if (isSelectedRow || isSelectedCol)
					cells.at(dataColIndex).at(dataRowIndex).draw(LIGHTGRAY);	// 在不编辑时若需要同行同列不高亮，则需要同时检验选中格是否被选中
				else
					cells.at(dataColIndex).at(dataRowIndex).draw();

			}
		}
		if (selectedCol != -1 && selectedRow != -1 && !isButton(selectedCol, selectedRow))		// 最后的条件似乎可以去掉
			cells.at(selectedCol).at(selectedRow).updateCursor();
	}
};

// 定义Widget类，表示一个简单的图形用户界面
class Widget {
private:
	int width; // 宽度
	int height; // 高度
	int currentIndex; // 当前页面索引
	vector<IMAGE*> pages;// 存储所有页面的图片指针
	vector<vector<Button*>> buttons;// 存储每个页面直属的按钮
	vector<vector<TextBox*>>textBoxs;// 存储每个页面直属的文本框
	vector<vector<TableBox*>>tableBoxs;// 存储每个页面直属的文本框表格

	// 添加一个页面
	void addPage(IMAGE* page) {
		pages.push_back(page);
		buttons.push_back({});
		textBoxs.push_back({});
		tableBoxs.push_back({});
	}

	// 在制定的页面上添加一个按钮
	void addButton(int index, Button* button) {
		if (index >= 0 && index < buttons.size())
			buttons[index].push_back(button);
	}

	// 在制定的页面上添加一个文本框
	void addTextBox(int index, TextBox* textbox) {
		if (index >= 0 && index < textBoxs.size())
			textBoxs[index].push_back(textbox);
	}

	// 在制定的页面上添加一个文本框按钮
	void addTableBox(int index, TableBox* tablebox) {
		if (index >= 0 && index < tableBoxs.size())
			tableBoxs[index].push_back(tablebox);
	}

	// 设置当前显示的页面索引
	void setCurrentIndex(int index) {
		if (index >= 0 && index < pages.size())
			currentIndex = index;
	}

	// 处理鼠标点击事件
	void mouseClick(int mouseX, int mouseY) {
		if (currentIndex >= 0 && currentIndex < buttons.size())
			for (Button*& button : buttons[currentIndex])
				if (button->checkClick(mouseX, mouseY))
					break;
		if (currentIndex >= 0 && currentIndex < textBoxs.size())
			for (TextBox*& textbox : textBoxs[currentIndex])
				textbox->checkClick(mouseX, mouseY);
		if (currentIndex >= 0 && currentIndex < tableBoxs.size())
			for (TableBox*& tablebox : tableBoxs[currentIndex])
				tablebox->handleMouseClick(mouseX, mouseY);
	}

	// 处理鼠标移动事件
	void mouseMove(int mouseX, int mouseY) {
		if (currentIndex >= 0 && currentIndex < buttons.size())
			for (Button*& button : buttons[currentIndex])
				button->checkMouseOver(mouseX, mouseY);
		if (currentIndex >= 0 && currentIndex < tableBoxs.size())
			for (TableBox*& tablebox : tableBoxs[currentIndex])
				tablebox->checkMouseOver(mouseX, mouseY);
	}

	// 处理鼠标滚动事件
	void mouseWheel(int mouseX, int mouseY, int wheel) {
		if (currentIndex >= 0 && currentIndex < tableBoxs.size())
			for (TableBox*& tablebox : tableBoxs[currentIndex])
				tablebox->scroll(mouseX, mouseY, wheel);
	}

	// 处理键盘输入事件
	void keyInput(char ch) {
		if (currentIndex >= 0 && currentIndex < textBoxs.size())
			for (TextBox*& textbox : textBoxs[currentIndex])
				if (textbox->getSelect())
					textbox->keyInput(ch);
		if (currentIndex >= 0 && currentIndex < tableBoxs.size())
			for (TableBox*& tablebox : tableBoxs[currentIndex])
				tablebox->keyInput(ch);
	}

	// 绘制当前页面的内容，取决于当前页面号
	void draw() {
		if (currentIndex >= 0 && currentIndex < pages.size()) {
			putimage(0, 0, pages[currentIndex]);				// 在窗口中绘制当前页面的图片
			if (currentIndex >= 0 && currentIndex < buttons.size())
				for (Button*& button : buttons[currentIndex])
					button->draw();
			if (currentIndex >= 0 && currentIndex < textBoxs.size())
				for (TextBox*& textbox : textBoxs[currentIndex]) {
					if (textbox->getSelect())
						textbox->updateCursor();
					textbox->draw();
				}
			if (currentIndex >= 0 && currentIndex < tableBoxs.size())
				for (TableBox*& tablebox : tableBoxs[currentIndex])
					tablebox->draw();
		}
	}

public:
	Widget(int width, int height)
		:width(width), height(height), currentIndex(-1) {}
	~Widget() {}

	// 初始化控件，创建图形环境，设置页面和按钮
	void init() {
		initgraph(width, height);

		// 创建页面1
		IMAGE* page1 = new IMAGE(width, height);
		setfillcolor(RGB(240, 240, 240));
		solidrectangle(0, 0, width, height);
		getimage(page1, 0, 0, width, height);
		addPage(page1);

		// 创建表格1
		TableBox* tablebox1 = new TableBox(10, 10, 600, 600, 10, 15);

		auto onButtonClickCalculate = [&] {
			MessageBox(GetHWnd(), "密码正确", "成功", MB_OK);
		};
		Button* buttonCalculate = new Button(230, 400, 50, 30, "确认", onButtonClickCalculate);
		tablebox1->toButton(*buttonCalculate, tablebox1->getCols() - 2, tablebox1->getRows() - 2);

		Button* buttonMaxMin = new Button(230, 400, 50, 30, "Max", {});
		tablebox1->toButton(*buttonMaxMin, 0, 0);
		Button* buttonOnScreen = tablebox1->getButton(0, 0);
		auto onButtonClickMaxMin = [=] {
			if (buttonOnScreen->getText() == "Max") {
				MessageBox(GetHWnd(), "切换至求Min", "更改优化目标", MB_OK);
				buttonOnScreen->setText("Min");
			}
			else if (buttonOnScreen->getText() == "Min") {
				MessageBox(GetHWnd(), "切换至求Max", "更改优化目标", MB_OK);
				buttonOnScreen->setText("Max");
			}
		};
		buttonOnScreen->setFunc(onButtonClickMaxMin);

		addTableBox(0, tablebox1);

		Button* buttonTest = new Button(700, 700, 50, 30, "确认", {});
		auto onButtonClickTest = [=] {
			MessageBox(GetHWnd(), "将切换至Min", "成功", MB_OK);
			buttonTest->setText("Min");
		};
		buttonTest->setFunc(onButtonClickTest);
		addButton(0, buttonTest);

		Button* buttonAddCol = new Button(700, 700, 50, 30, "新增", {});
		tablebox1->toButton(*buttonAddCol, tablebox1->getCols() - 1, 0);
		buttonOnScreen = tablebox1->getButton(tablebox1->getCols() - 1, 0);
		auto onButtonClickAddCol = [=] {
			tablebox1->addCol(tablebox1->getCols()-1);
			tablebox1->scrollRight();
			//int now_col = tablebox1->getCols();
			//MessageBox(GetHWnd(), ("当前共"+to_string(now_col)+"列").c_str(), "新增列", MB_OK);
		};
		buttonOnScreen->setFunc(onButtonClickAddCol);

		Button* buttonAddRow = new Button(700, 700, 50, 30, "新增", {});
		tablebox1->toButton(*buttonAddRow, 0, tablebox1->getRows() - 1);
		buttonOnScreen = tablebox1->getButton(0, tablebox1->getRows() - 1);
		auto onButtonClickAddRow = [=] {
			tablebox1->addRow(tablebox1->getRows() - 1); 
			tablebox1->scrollDown();
			//int now_row = tablebox1->getRows();
			//MessageBox(GetHWnd(), ("当前共" + to_string(now_row) + "行").c_str(), "新增行", MB_OK);
		};
		buttonOnScreen->setFunc(onButtonClickAddRow);

		/*
		// 在界面1创建按钮1
		auto onButtonClicked = [&] {
			if (textBoxs[0][0]->getText() != "你好世界") {
				MessageBox(GetHWnd(), "密码错误", "警告", MB_ICONWARNING | MB_OK);
			}
			else {
				MessageBox(GetHWnd(), "密码正确", "成功", MB_OK);
				textBoxs[0][0]->clear();
				setCurrentIndex(1);
			}
		};
		Button* button1_1 = new Button(230, 400, 50, 30, "确认", onButtonClicked);
		addButton(0, button1_1);

		//在界面1创建文本框1
		TextBox* textbox1_1 = new TextBox(200, 300, 120, 30, 8);
		addTextBox(0, textbox1_1);

		// 创建页面2
		IMAGE* page2 = new IMAGE(width, height);
		setfillcolor(RED);
		solidrectangle(0, 0, width, height);
		getimage(page2, 0, 0, width, height);
		addPage(page2);

		// 在界面2创建按钮1
		Button* button2_1 = new Button(100, 200, 100, 50, "返回到Page 1", [&]() {
			setCurrentIndex(0);
			});
		addButton(1, button2_1);
		*/

		setCurrentIndex(0);
	}

	// 运行，进入消息循环
	void run() {
		ExMessage msg;

		BeginBatchDraw();	// 开始批量绘制
		while (true) {
			if (peekmessage(&msg)) {// 检查是否有消息
				int mouseX = msg.x;
				int mouseY = msg.y;
				switch (msg.message) {
				case WM_LBUTTONDOWN:
					mouseClick(mouseX, mouseY);
					break;
				case WM_MOUSEMOVE:
					mouseMove(mouseX, mouseY);
					break;
				case WM_MOUSEWHEEL:
					mouseWheel(mouseX, mouseY, msg.wheel);
					break;
				case WM_CHAR:
					keyInput(msg.ch);
					break;
				}
			}
			draw();
			FlushBatchDraw();// 将缓冲区内容显示在屏幕上
			Sleep(10);
		}
		EndBatchDraw();// 结束批量绘制
	}

	void close() { closegraph(); }
};

