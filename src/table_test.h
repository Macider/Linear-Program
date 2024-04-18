#pragma once

#include <graphics.h>
#include <string>
#include <vector>
using namespace std;

//表格类
class TableWidget {
private:
    int x;
    int y;
    int width;
    int height;
    int visibleRowCount;//最多可见行数
    int rowHeight;      //行的高度
    int scrollOffset;   //偏移
    vector<vector<string>> data;
    vector<int> columnWidths;//列的宽度
    int selectedRow;        //被选中的行
    int scrollbarWidth;
    int handleHeight;
    int handleY;

public:
    TableWidget(int x, int y, int width, int height, int visibleRowCount)
        : x(x), y(y), width(width), height(height), visibleRowCount(visibleRowCount) {
        data = { {} };
        rowHeight = height / visibleRowCount;
        selectedRow = -1;
        scrollOffset = 0;
        scrollbarWidth = 20;
        handleHeight = 30;
        handleY = 0;
    }

    int getSelectedRow() const
    {
        return selectedRow;
    }

    string getSelectedInfo(int col) const
    {
        return data[selectedRow][col];
    }

    void setData(const vector<vector<string>>& newData)
    {
        data = newData;
        calculateColumnWidths();
    }

    //计算每列的宽度
    void calculateColumnWidths()
    {
        columnWidths.clear();
        if (!data.empty())
        {
            columnWidths.resize(data[0].size(), 0);
            for (const auto& row : data)
            {
                for (size_t j = 0; j < row.size(); ++j)
                {
                    int width = textwidth(row[j].c_str());
                    if (width > columnWidths[j])    //每列的最大宽度（即每列中字符最大长度）
                    {
                        columnWidths[j] = width;
                    }
                }
            }
        }
        //根据每列最大宽度计算该列长度
        int sumWidth = 0;
        for (auto it : columnWidths)
            sumWidth += it;
        for (auto& it : columnWidths)
            it = (float)it / sumWidth * width;
    }

    void scrollUp()
    {
        if (scrollOffset > 0)
        {
            scrollOffset--;
        }
        if (scrollOffset < 0)
        {
            scrollOffset = 0;
        }
    }

    void scrollDown()
    {
        int maxScrollOffset = data.size() - visibleRowCount;
        if (scrollOffset < maxScrollOffset)
        {
            scrollOffset++;
        }
    }

    void scroll(int mouseX, int mouseY, int wheel)
    {
        if (mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height)
        {
            if (wheel > 0)
            {
                scrollUp();
            }
            else if (wheel < 0) {
                scrollDown();
            }
        }
    }

    void handleMouseClick(int mouseX, int mouseY)
    {
        if (mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height)
        {
            int clickedRow = (mouseY - y) / rowHeight + scrollOffset;

            if (clickedRow >= 0 && clickedRow < static_cast<int>(data.size()))
            {
                selectedRow = clickedRow;
            }
            else
            {
                selectedRow = -1;
            }
        }
    }

    void draw()
    {
        setbkmode(1);
        // 绘制表格区域
        setfillcolor(WHITE);
        solidrectangle(x, y, x + width, y + height);
        setlinecolor(BLACK);
        settextstyle(12, 0, "宋体");

        // 计算需要绘制的行数
        int rowCount = min(visibleRowCount, static_cast<int>(data.size()));

        // 绘制表头
        int headerY = y;
        int columnX = x;
        for (int j = 0; j < data[0].size(); ++j) {
            int columnWidth = columnWidths[j];
            rectangle(columnX, headerY, columnX + columnWidth, headerY + rowHeight);
            int textX = columnX + (columnWidth - textwidth(data[0][j].c_str())) / 2;
            int textY = headerY + (rowHeight - textheight("宋体")) / 2;
            outtextxy(textX, textY, data[0][j].c_str());
            columnX += columnWidth;
        }

        // 绘制表格内容
        for (int i = 1; i < rowCount; ++i) {
            int rowY = y + i * rowHeight;
            int dataIndex = i + scrollOffset;
            columnX = x;
            for (int j = 0; j < data[dataIndex].size(); ++j) {
                int columnWidth = columnWidths[j];
                bool isSelectedRow = (dataIndex == selectedRow);
                if (isSelectedRow) {
                    setfillcolor(LIGHTBLUE);
                    settextcolor(RED);
                }
                else {
                    setfillcolor(WHITE);
                    settextcolor(BLACK);
                }
                fillrectangle(columnX, rowY, columnX + columnWidth, rowY + rowHeight);
                int textX = columnX + (columnWidth - textwidth(data[dataIndex][j].c_str())) / 2;
                int textY = rowY + (rowHeight - textheight("宋体")) / 2;
                outtextxy(textX, textY, data[dataIndex][j].c_str());
                columnX += columnWidth;
            }
        }
        // 绘制滚动条背景
        int scrollbarX = x + width;
        setfillcolor(LIGHTGRAY);
        solidrectangle(scrollbarX, y, scrollbarX + scrollbarWidth, y + height);

        // 计算滑块位置和大小
        int handleX = scrollbarX;
        int handleWidth = scrollbarWidth;
        int maxHandleY = height - handleHeight;
        handleY = maxHandleY * double(scrollOffset) / (double(data.size()) - double(visibleRowCount));

        // 绘制滑块
        setfillcolor(DARKGRAY);
        solidrectangle(handleX, y + handleY, handleX + handleWidth, y + handleY + handleHeight);
    }
};


class Widget
{
private:
    int width;
    int height;
    int currentIndex;
    vector<IMAGE*> pages;
    vector<vector<TableWidget*>> tables;

    void addPage(IMAGE* page)
    {
        pages.push_back(page);
        tables.push_back({});
    }

    void addTable(int index, TableWidget* button)
    {
        if (index >= 0 && index < tables.size())
        {
            tables[index].push_back(button);
        }
    }

    void mouseClick(int mouseX, int mouseY)
    {
        if (currentIndex >= 0 && currentIndex < tables.size())
        {
            for (TableWidget* table : tables[currentIndex])
            {
                table->handleMouseClick(mouseX, mouseY);
            }
        }
    }

    void mouseWheel(int mouseX, int mouseY, int wheel)
    {

        if (currentIndex >= 0 && currentIndex < tables.size())
        {
            for (TableWidget* table : tables[currentIndex])
            {
                table->scroll(mouseX, mouseY, wheel);
            }
        }
    }

    void draw()
    {
        if (currentIndex >= 0 && currentIndex < tables.size())
        {
            putimage(0, 0, pages[currentIndex]);
            for (TableWidget* table : tables[currentIndex])
            {
                table->draw();
            }
        }
    }

public:
    Widget(int width, int height)
        :width(width), height(height), currentIndex(-1)
    {
    }
    ~Widget() {}

    void init()
    {
        initgraph(width, height);

        // 创建页面1
        IMAGE* page1 = new IMAGE(width, height);
        setfillcolor(RGB(240, 240, 240));
        solidrectangle(0, 0, width, height);
        getimage(page1, 0, 0, width, height);
        addPage(page1);

        TableWidget* table1 = new TableWidget(100, 100, 400, 300, 12);
        table1->setData({ {"学号","姓名","性别","成绩"},
            {"2023001","张三","男","60"},
            {"2023002","李四","男","50"},
            {"2023003","王五","男","90"},
            {"2023004","小红","女","80"},
            {"2023005","小黑","男","70"},
            {"2023006","小白","男","60"},
            {"2023007","李红","女","75"},
            {"2023008","王五","男","60"},
            {"2023009","王五","女","40"},
            {"2023010","王五","男","30"},
            {"2023011","王五","男","20"},
            {"2023012","王五","女","10"},
            {"2023013","王五","男","80"},
            {"2023014","王五","女","90"},
            {"2023015","王五","女","100"},
            });
        /*
            */
        addTable(0, table1);

        currentIndex = 0;
    }

    void run()
    {
        ExMessage msg;

        BeginBatchDraw();
        while (true)
        {
            if (peekmessage(&msg))
            {
                int mouseX = msg.x;
                int mouseY = msg.y;
                switch (msg.message)
                {
                case WM_LBUTTONDOWN:
                    mouseClick(mouseX, mouseY);
                    break;
                case WM_MOUSEWHEEL:
                    mouseWheel(mouseX, mouseY, msg.wheel);
                    break;
                }
            }
            draw();
            FlushBatchDraw();
            Sleep(10);
        }
        EndBatchDraw();
    }

    void close()
    {
        closegraph();
    }
};
/*
int main()
{
    Widget widget(800, 600);
    widget.init();
    widget.run();
    widget.close();
    return 0;
}
*/
