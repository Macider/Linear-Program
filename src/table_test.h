#pragma once

#include <graphics.h>
#include <string>
#include <vector>
using namespace std;

//�����
class TableWidget {
private:
    int x;
    int y;
    int width;
    int height;
    int visibleRowCount;//���ɼ�����
    int rowHeight;      //�еĸ߶�
    int scrollOffset;   //ƫ��
    vector<vector<string>> data;
    vector<int> columnWidths;//�еĿ��
    int selectedRow;        //��ѡ�е���
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

    //����ÿ�еĿ��
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
                    if (width > columnWidths[j])    //ÿ�е�����ȣ���ÿ�����ַ���󳤶ȣ�
                    {
                        columnWidths[j] = width;
                    }
                }
            }
        }
        //����ÿ������ȼ�����г���
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
        // ���Ʊ������
        setfillcolor(WHITE);
        solidrectangle(x, y, x + width, y + height);
        setlinecolor(BLACK);
        settextstyle(12, 0, "����");

        // ������Ҫ���Ƶ�����
        int rowCount = min(visibleRowCount, static_cast<int>(data.size()));

        // ���Ʊ�ͷ
        int headerY = y;
        int columnX = x;
        for (int j = 0; j < data[0].size(); ++j) {
            int columnWidth = columnWidths[j];
            rectangle(columnX, headerY, columnX + columnWidth, headerY + rowHeight);
            int textX = columnX + (columnWidth - textwidth(data[0][j].c_str())) / 2;
            int textY = headerY + (rowHeight - textheight("����")) / 2;
            outtextxy(textX, textY, data[0][j].c_str());
            columnX += columnWidth;
        }

        // ���Ʊ������
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
                int textY = rowY + (rowHeight - textheight("����")) / 2;
                outtextxy(textX, textY, data[dataIndex][j].c_str());
                columnX += columnWidth;
            }
        }
        // ���ƹ���������
        int scrollbarX = x + width;
        setfillcolor(LIGHTGRAY);
        solidrectangle(scrollbarX, y, scrollbarX + scrollbarWidth, y + height);

        // ���㻬��λ�úʹ�С
        int handleX = scrollbarX;
        int handleWidth = scrollbarWidth;
        int maxHandleY = height - handleHeight;
        handleY = maxHandleY * double(scrollOffset) / (double(data.size()) - double(visibleRowCount));

        // ���ƻ���
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

        // ����ҳ��1
        IMAGE* page1 = new IMAGE(width, height);
        setfillcolor(RGB(240, 240, 240));
        solidrectangle(0, 0, width, height);
        getimage(page1, 0, 0, width, height);
        addPage(page1);

        TableWidget* table1 = new TableWidget(100, 100, 400, 300, 12);
        table1->setData({ {"ѧ��","����","�Ա�","�ɼ�"},
            {"2023001","����","��","60"},
            {"2023002","����","��","50"},
            {"2023003","����","��","90"},
            {"2023004","С��","Ů","80"},
            {"2023005","С��","��","70"},
            {"2023006","С��","��","60"},
            {"2023007","���","Ů","75"},
            {"2023008","����","��","60"},
            {"2023009","����","Ů","40"},
            {"2023010","����","��","30"},
            {"2023011","����","��","20"},
            {"2023012","����","Ů","10"},
            {"2023013","����","��","80"},
            {"2023014","����","Ů","90"},
            {"2023015","����","Ů","100"},
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
