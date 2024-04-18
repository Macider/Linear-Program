#pragma once

#include <easyx.h>
#include <functional>
#include <string>
#include <vector>
#include <iostream>
#include <cassert>

using namespace std;

// ����Button�࣬��ʾһ����ť
class Button {
private:
	int x; // ��ť���Ͻ�x����
	int y; // ��ť���Ͻ�y����
	int width; // ��ť���
	int height; // ��ť�߶�
	float scale; // ���ű���������ʵ�������ͣЧ��
	bool isMouseOver; // ��ʾ����Ƿ��ڰ�ť�Ϸ�
	string text; // ��ť�ı�
	function<void()> onClick; // �����ť�����ĺ�����Ҫ���޲��������޷���ֵ

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

	// �������Ƿ��ڰ�ť�Ϸ�
	void checkMouseOver(int mouseX, int mouseY) {
		isMouseOver = (mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height);
		if (isMouseOver)
			scale = 0.9f; // �����ͣʱ���Ű�ť
		else
			scale = 1.0f; // �ָ���ťԭʼ��С
	}

	// ���������Ƿ��ڰ�ť�ڣ���ִ�к���
	bool checkClick(int mouseX, int mouseY) {
		if (mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height) {
			onClick(); // ִ�а�ť���ʱ�ĺ���
			isMouseOver = false;
			scale = 1.0f;
			return true;
		}
		return false;
	}

	// ���ư�ť
	void draw() {
		int scaledWidth = width * scale; // ���ź�İ�ť���
		int scaledHeight = height * scale; // ���ź�İ�ť�߶�
		int scaledX = x + (width - scaledWidth) / 2; // ���ź�İ�ťx����
		int scaledY = y + (height - scaledHeight) / 2; // ���ź�İ�ťy����

		if (isMouseOver) {
			setlinecolor(RGB(0, 120, 215)); // �����ͣʱ��ť�߿���ɫ
			setfillcolor(RGB(229, 241, 251)); // �����ͣʱ��ť�����ɫ
		}
		else {
			setlinecolor(RGB(173, 173, 173)); // ��ť�߿���ɫ
			setfillcolor(RGB(225, 225, 225)); // ��ť�����ɫ
		}

		fillrectangle(scaledX, scaledY, scaledX + scaledWidth, scaledY + scaledHeight); // ���ư�ť
		settextcolor(BLACK); // �����ı���ɫΪ��ɫ
		setbkmode(TRANSPARENT); // �����ı�����͸��
		settextstyle(20 * scale, 0, "����"); // �����ı���С������
		//������ʾ��ť�ı�
		int textX = scaledX + (scaledWidth - textwidth(text.c_str())) / 2; // �����ı��ڰ�ť�����x����
		int textY = scaledY + (scaledHeight - textheight("����")) / 2; // �����ı��ڰ�ť�����y����
		outtextxy(textX, textY, text.c_str()); // �ڰ�ť�ϻ����ı�
	}
};

// ����TextBox�࣬��ʾһ���ı���
class TextBox {
private:
	int x;
	int y;
	int width;
	int height;
	int maxWord;    //����ʸ���
	string text;   //�ı�
	bool isSelected;    //�Ƿ�ѡ��
	bool showCursor;    //�Ƿ���ʾ���
	int cursorPos;      //���λ��

public:
	TextBox(int x, int y, int width, int height, int maxWord)
		: x(x), y(y), width(width), height(height), maxWord(maxWord), isSelected(false), showCursor(false), cursorPos(0) {}

	const string& getText() const { return text; }		// ���ش�����ı�����

	bool getSelect() const { return isSelected; }		// �����Ƿ�ѡ��

	bool isEmpty() const { return text.empty(); }		// �ı����Ƿ�Ϊ��

	void clear() { text = ""; }							// ����ı�������

	void setText(const string& newText) {
		text = newText;
		cursorPos = text.length();
	}

	// ��Ҫ���޸�xy�Ľӿ�
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
		fillrectangle(x, y, x + width, y + height);		// �ı������

		settextcolor(BLACK);
		setbkmode(TRANSPARENT);
		settextstyle(height * 3 / 4, 0, "����");

		outtextxy(x + 5, y + (height - textheight("����")) / 2, text.c_str());

		setlinecolor(BLACK);
		if (isSelected && showCursor) {					// ���ƹ��
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
			case '\b':				// �û����˸����ɾ��һ���ַ�
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
			default:				// �û����������������ı�����
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
		if (currentTick - lastTick >= 500) { // ÿ500�����л����״̬
			showCursor = !showCursor;
			lastTick = currentTick;
		}
	}
};

// ����TableBox�࣬��ʾһ�����ı��򼰰�ť��ɵı��
class TableBox {
private:
	int x, y;								// ������Ͻ�����
	int width, height;						// ����ȡ��߶�
	int rowHeight, colWidth;				// �иߡ��п�
	int visibleRowCount, visibleColCount;	// ���ɼ��С�����
	int selectedRow, selectedCol;			// ��ѡ�е��С���
	int rows, cols;							// ���С�����
	vector<vector<TextBox>> cells;			// ���д���
	typedef pair<Button, pair<int, int>> ButtonInform;
	vector<ButtonInform> buttonsInform;		// ���ָ��ӿ��Ը�Ϊ��ť
	// ���򻬶������
	int scrollRowOffset;
	int scrollbarWidth;		// ���򻬶������
	int handleHeight;		// ���򻬿�߶�
	int handleY;			// ���򻬿�����λ��
	// ���򻬶������
	int scrollColOffset;
	int scrollbarHeight;	// ���򻬶����߶�
	int handleWidth;		// ���򻬿���
	int handleX;			// ���򻬿�����λ��

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

	// ���¼���������ӵ�λ�ü���С
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

	void addCol(int index = -1) {	// ��[index]��ǰ������У�Ĭ�������
		assert(-1 <= index && index <= cols);
		cols++;
		int colCount = min(visibleColCount, cols);
		colWidth = width / colCount;
		vector<TextBox> col;
		if (index == -1 || index == cols - 1) {
			for (int i = 0; i < rows; ++i)
				col.push_back(TextBox(x + (cols - 1) * colWidth, y + i * rowHeight, colWidth, rowHeight, 100));
			cells.push_back(col);	// ��push��col���±�Ϊcols-1

		}
		else {				// index in [0,cols-2]
			for (int i = 0; i < rows; ++i)
				col.push_back(TextBox(x + index * colWidth, y + i * rowHeight, colWidth, rowHeight, 100));
			cells.emplace(cells.begin() + index, col);	//�²����col�±�Ϊindex
			// �±�>=index��button�ڱ���е����λ�û�ı�
			for (ButtonInform& buttonInform : buttonsInform)
				if (buttonInform.second.first>=index)
					buttonInform.second.first++;
		
		}
		// ������仯ʱ����У׼
		calibrateTable();
	}
	void addRow(int index = -1) {	// ��[index]��ǰ������У�Ĭ�������
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
			// �±�>=index��button�ڱ���е����λ�û�ı�
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
		// ������仯ʱ����У׼
		calibrateTable();
	}

	void deletaCol(int index = -1) {	// ɾ��[index%cols]
	}
	void deleteRow(int index = -1) {	// ɾ��[index%rows]
	}

	// ��ĳ����Ԫ������Ϊ��ť
	void toButton(Button button, int col, int row) {
		pair<int, int> colAndRow = make_pair(col, row);
		button.setWH(colWidth, rowHeight);
		button.setXY(x + col * colWidth, y + row * rowHeight);
		buttonsInform.push_back(make_pair(button, colAndRow));
	}
	// ���ĳ����Ԫ���Ƿ�Ϊ��ť
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

	// ���򻬶����ϻ�
	void scrollUp() {
		if (scrollRowOffset <= 0) {
			scrollRowOffset = 0;
			return;
		}
		scrollRowOffset--;
		// �������仯ʱ����У׼
		calibrateTable();
	}
	// ���򻬶����»�
	void scrollDown() {
		int maxScrollOffset = max(rows - visibleRowCount, 0);
		if (scrollRowOffset >= maxScrollOffset) {
			scrollRowOffset = maxScrollOffset;
			return;
		}
		scrollRowOffset++;
		// �������仯ʱ����У׼
		calibrateTable();
	}
	// ���򻬶�����
	void scrollLeft() {
		if (scrollColOffset <= 0) {
			scrollColOffset = 0;
			return;
		}
		scrollColOffset--;
		// �������仯ʱ����У׼
		calibrateTable();
	}
	// ���򻬶����һ�
	void scrollRight() {
		int maxScrollOffset = max(cols - visibleColCount, 0);
		if (scrollColOffset >= maxScrollOffset) {
			scrollColOffset = maxScrollOffset;
			return;
		}
		scrollColOffset++;
		// �������仯ʱ����У׼
		calibrateTable();
	}
	// ��������
	void scroll(int mouseX, int mouseY, int wheel) {
		// ���򻬶���
		bool changeOrNot = false;
		if (mouseX >= x + width && mouseX <= x + width + scrollbarWidth && mouseY >= y && mouseY <= y + height) {
			changeOrNot = true;
			if (wheel > 0)
				scrollUp();
			else if (wheel < 0)
				scrollDown();
		}
		// ���򻬶���
		else if (mouseX >= x && mouseX <= x + width && mouseY >= y + height && mouseY <= y + height + scrollbarHeight) {
			changeOrNot = true;
			if (wheel > 0)
				scrollLeft();
			else if (wheel < 0)
				scrollRight();
		}
	}

	// ��������ƶ�
	void checkMouseOver(int mouseX, int mouseY) {
		for (ButtonInform& buttonInform : buttonsInform)
			buttonInform.first.checkMouseOver(mouseX, mouseY);
		if (mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height) {
			int dataColIndex = (mouseX - x) / colWidth + scrollColOffset;
			int dataRowIndex = (mouseY - y) / rowHeight + scrollRowOffset;
			// �ƺ���������
		}
	}

	// ���������
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

	// �����������
	void keyInput(char ch) {
		if (selectedCol == -1 || selectedRow == -1)
			return;
		assert(cells.at(selectedCol).at(selectedRow).getSelect());
		cells.at(selectedCol).at(selectedRow).keyInput(ch);
	}

	void draw() {
		setbkmode(TRANSPARENT);		// �������ֱ���Ϊ͸��
		// ���Ʊ������
		setfillcolor(WHITE);
		solidrectangle(x, y, x + width, y + height);
		setlinecolor(BLACK);
		settextstyle(12, 0, "����");

		// ���ƹ���������
		int scrollbarX = x + width;		// ����ұ߽�
		int scrollbarY = y + height;	// ����±߽�
		setfillcolor(LIGHTGRAY);
		solidrectangle(scrollbarX, y, scrollbarX + scrollbarWidth, scrollbarY);
		solidrectangle(x, scrollbarY, scrollbarX, scrollbarY + scrollbarHeight);

		int handlePosX, handlePosY;
		setfillcolor(DARKGRAY);
		// ����λ�ô�С���������򻬿�
		handlePosX = scrollbarX;
		int maxHandleY = height - handleHeight;		// ʵ�����ƶ��ľ���
		handlePosY = y + maxHandleY * double(scrollRowOffset) / max(rows - visibleRowCount, 1);
		solidrectangle(handlePosX, handlePosY, handlePosX + scrollbarWidth, handlePosY + handleHeight);
		// ����λ�ô�С�����ƺ��򻬿�
		handlePosY = scrollbarY;
		int maxHandleX = width - handleWidth;		// ʵ�����ƶ��ľ���
		handlePosX = x + maxHandleX * double(scrollColOffset) / max(cols - visibleColCount, 1);
		solidrectangle(handlePosX, handlePosY, handlePosX + handleWidth, handlePosY + scrollbarHeight);



		// ������Ҫ���Ƶ��С�����
		int rowCount = min(visibleRowCount, rows);
		int colCount = min(visibleColCount, cols);

		// ���Ʊ��й̶�����


		// ���Ʊ����������
		for (int j = 0; j < colCount; ++j) {
			int colX = x + j * colWidth;
			int dataColIndex = j + scrollColOffset;
			bool isSelectedCol = (dataColIndex == selectedCol);
			for (int i = 0; i < rowCount; ++i) {
				int rowY = y + i * rowHeight;
				int dataRowIndex = i + scrollRowOffset;
				bool isSelectedRow = (dataRowIndex == selectedRow);

				bool isButton = false;
				for (ButtonInform& buttonInform : buttonsInform) {		// Ĭ�ϴ���������������Ҫ��&
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
					cells.at(dataColIndex).at(dataRowIndex).draw(LIGHTGRAY);	// �ڲ��༭ʱ����Ҫͬ��ͬ�в�����������Ҫͬʱ����ѡ�и��Ƿ�ѡ��
				else
					cells.at(dataColIndex).at(dataRowIndex).draw();

			}
		}
		if (selectedCol != -1 && selectedRow != -1 && !isButton(selectedCol, selectedRow))		// ���������ƺ�����ȥ��
			cells.at(selectedCol).at(selectedRow).updateCursor();
	}
};

// ����Widget�࣬��ʾһ���򵥵�ͼ���û�����
class Widget {
private:
	int width; // ���
	int height; // �߶�
	int currentIndex; // ��ǰҳ������
	vector<IMAGE*> pages;// �洢����ҳ���ͼƬָ��
	vector<vector<Button*>> buttons;// �洢ÿ��ҳ��ֱ���İ�ť
	vector<vector<TextBox*>>textBoxs;// �洢ÿ��ҳ��ֱ�����ı���
	vector<vector<TableBox*>>tableBoxs;// �洢ÿ��ҳ��ֱ�����ı�����

	// ���һ��ҳ��
	void addPage(IMAGE* page) {
		pages.push_back(page);
		buttons.push_back({});
		textBoxs.push_back({});
		tableBoxs.push_back({});
	}

	// ���ƶ���ҳ�������һ����ť
	void addButton(int index, Button* button) {
		if (index >= 0 && index < buttons.size())
			buttons[index].push_back(button);
	}

	// ���ƶ���ҳ�������һ���ı���
	void addTextBox(int index, TextBox* textbox) {
		if (index >= 0 && index < textBoxs.size())
			textBoxs[index].push_back(textbox);
	}

	// ���ƶ���ҳ�������һ���ı���ť
	void addTableBox(int index, TableBox* tablebox) {
		if (index >= 0 && index < tableBoxs.size())
			tableBoxs[index].push_back(tablebox);
	}

	// ���õ�ǰ��ʾ��ҳ������
	void setCurrentIndex(int index) {
		if (index >= 0 && index < pages.size())
			currentIndex = index;
	}

	// ����������¼�
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

	// ��������ƶ��¼�
	void mouseMove(int mouseX, int mouseY) {
		if (currentIndex >= 0 && currentIndex < buttons.size())
			for (Button*& button : buttons[currentIndex])
				button->checkMouseOver(mouseX, mouseY);
		if (currentIndex >= 0 && currentIndex < tableBoxs.size())
			for (TableBox*& tablebox : tableBoxs[currentIndex])
				tablebox->checkMouseOver(mouseX, mouseY);
	}

	// �����������¼�
	void mouseWheel(int mouseX, int mouseY, int wheel) {
		if (currentIndex >= 0 && currentIndex < tableBoxs.size())
			for (TableBox*& tablebox : tableBoxs[currentIndex])
				tablebox->scroll(mouseX, mouseY, wheel);
	}

	// ������������¼�
	void keyInput(char ch) {
		if (currentIndex >= 0 && currentIndex < textBoxs.size())
			for (TextBox*& textbox : textBoxs[currentIndex])
				if (textbox->getSelect())
					textbox->keyInput(ch);
		if (currentIndex >= 0 && currentIndex < tableBoxs.size())
			for (TableBox*& tablebox : tableBoxs[currentIndex])
				tablebox->keyInput(ch);
	}

	// ���Ƶ�ǰҳ������ݣ�ȡ���ڵ�ǰҳ���
	void draw() {
		if (currentIndex >= 0 && currentIndex < pages.size()) {
			putimage(0, 0, pages[currentIndex]);				// �ڴ����л��Ƶ�ǰҳ���ͼƬ
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

	// ��ʼ���ؼ�������ͼ�λ���������ҳ��Ͱ�ť
	void init() {
		initgraph(width, height);

		// ����ҳ��1
		IMAGE* page1 = new IMAGE(width, height);
		setfillcolor(RGB(240, 240, 240));
		solidrectangle(0, 0, width, height);
		getimage(page1, 0, 0, width, height);
		addPage(page1);

		// �������1
		TableBox* tablebox1 = new TableBox(10, 10, 600, 600, 10, 15);

		auto onButtonClickCalculate = [&] {
			MessageBox(GetHWnd(), "������ȷ", "�ɹ�", MB_OK);
		};
		Button* buttonCalculate = new Button(230, 400, 50, 30, "ȷ��", onButtonClickCalculate);
		tablebox1->toButton(*buttonCalculate, tablebox1->getCols() - 2, tablebox1->getRows() - 2);

		Button* buttonMaxMin = new Button(230, 400, 50, 30, "Max", {});
		tablebox1->toButton(*buttonMaxMin, 0, 0);
		Button* buttonOnScreen = tablebox1->getButton(0, 0);
		auto onButtonClickMaxMin = [=] {
			if (buttonOnScreen->getText() == "Max") {
				MessageBox(GetHWnd(), "�л�����Min", "�����Ż�Ŀ��", MB_OK);
				buttonOnScreen->setText("Min");
			}
			else if (buttonOnScreen->getText() == "Min") {
				MessageBox(GetHWnd(), "�л�����Max", "�����Ż�Ŀ��", MB_OK);
				buttonOnScreen->setText("Max");
			}
		};
		buttonOnScreen->setFunc(onButtonClickMaxMin);

		addTableBox(0, tablebox1);

		Button* buttonTest = new Button(700, 700, 50, 30, "ȷ��", {});
		auto onButtonClickTest = [=] {
			MessageBox(GetHWnd(), "���л���Min", "�ɹ�", MB_OK);
			buttonTest->setText("Min");
		};
		buttonTest->setFunc(onButtonClickTest);
		addButton(0, buttonTest);

		Button* buttonAddCol = new Button(700, 700, 50, 30, "����", {});
		tablebox1->toButton(*buttonAddCol, tablebox1->getCols() - 1, 0);
		buttonOnScreen = tablebox1->getButton(tablebox1->getCols() - 1, 0);
		auto onButtonClickAddCol = [=] {
			tablebox1->addCol(tablebox1->getCols()-1);
			tablebox1->scrollRight();
			//int now_col = tablebox1->getCols();
			//MessageBox(GetHWnd(), ("��ǰ��"+to_string(now_col)+"��").c_str(), "������", MB_OK);
		};
		buttonOnScreen->setFunc(onButtonClickAddCol);

		Button* buttonAddRow = new Button(700, 700, 50, 30, "����", {});
		tablebox1->toButton(*buttonAddRow, 0, tablebox1->getRows() - 1);
		buttonOnScreen = tablebox1->getButton(0, tablebox1->getRows() - 1);
		auto onButtonClickAddRow = [=] {
			tablebox1->addRow(tablebox1->getRows() - 1); 
			tablebox1->scrollDown();
			//int now_row = tablebox1->getRows();
			//MessageBox(GetHWnd(), ("��ǰ��" + to_string(now_row) + "��").c_str(), "������", MB_OK);
		};
		buttonOnScreen->setFunc(onButtonClickAddRow);

		/*
		// �ڽ���1������ť1
		auto onButtonClicked = [&] {
			if (textBoxs[0][0]->getText() != "�������") {
				MessageBox(GetHWnd(), "�������", "����", MB_ICONWARNING | MB_OK);
			}
			else {
				MessageBox(GetHWnd(), "������ȷ", "�ɹ�", MB_OK);
				textBoxs[0][0]->clear();
				setCurrentIndex(1);
			}
		};
		Button* button1_1 = new Button(230, 400, 50, 30, "ȷ��", onButtonClicked);
		addButton(0, button1_1);

		//�ڽ���1�����ı���1
		TextBox* textbox1_1 = new TextBox(200, 300, 120, 30, 8);
		addTextBox(0, textbox1_1);

		// ����ҳ��2
		IMAGE* page2 = new IMAGE(width, height);
		setfillcolor(RED);
		solidrectangle(0, 0, width, height);
		getimage(page2, 0, 0, width, height);
		addPage(page2);

		// �ڽ���2������ť1
		Button* button2_1 = new Button(100, 200, 100, 50, "���ص�Page 1", [&]() {
			setCurrentIndex(0);
			});
		addButton(1, button2_1);
		*/

		setCurrentIndex(0);
	}

	// ���У�������Ϣѭ��
	void run() {
		ExMessage msg;

		BeginBatchDraw();	// ��ʼ��������
		while (true) {
			if (peekmessage(&msg)) {// ����Ƿ�����Ϣ
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
			FlushBatchDraw();// ��������������ʾ����Ļ��
			Sleep(10);
		}
		EndBatchDraw();// ������������
	}

	void close() { closegraph(); }
};

