#include "all_definitions.h"
#include "all_inclusions.h"

using namespace std;

string CurlDownloadPage(char[]);
static int Write(char*, size_t, size_t, string*);
bool PageParser(string);
bool IsItNumb(char);
int StringToNumb(string);

int main()
{
    char pageUrl[] = "https://www.wowhead.com/items";
    string page = CurlDownloadPage(pageUrl);

    PageParser(page);

    system("pause");
    return 0;
}

string CurlDownloadPage(char pageUrl[])
{
    string content;

    CurlGlobalInit(CURL_GLOBAL_ALL);
    CURL* curl = nullptr;
    curl = CurlEasyInit();
    if (curl)
    {
        CurlEasySetopt(curl, CURLOPT_URL, pageUrl);
        CurlEasySetopt(curl, CURLOPT_WRITEDATA, &content);
        CurlEasySetopt(curl, CURLOPT_WRITEFUNCTION, Write);
        CURLcode code = CurlEasyPerform(curl);
        CurlEasyCleanup(curl);
    }
    CurlGlobalCleanup();

    return content;
}

static int Write(char* data, size_t size, size_t nmemb, string* writerData)
{
    if (writerData == NULL)
        return 0;

    writerData->append(data, size * nmemb);

    return size * nmemb;
}

bool PageParser(string page)
{
    libxl::Book* book = xlCreateBook();
    if (book)
    {
        libxl::Sheet* sheet = book->addSheet(L"Sheet");
        if (sheet)
        {
            //Indexes of left and right quote marks (initialization 0)
            int quote_mark_L = 0;       int quote_mark_R = 0;
            //Left and Right bracket count
            int bracket_count_L = -1;   int bracket_count_R = 0;

            //String of the cell
            string cell_str;
            //Cell of W-string, for writing sheet->writeStr(table_row, table_column, cell_str.c_str());
            wstring w_cell_str;
            //String of numbers 
            string numb_str = "";

            wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;

            //Row and Column Coordinates
            int table_row = 2;      //  --
            int table_column = 0;   //  |

            //Jsonequip subarray
            int json_start = 0; int json_end = 0;

            //Initializing temporary table columns
            sheet->writeStr(1, 0, L"id");
            sheet->writeStr(1, 1, L"name_enus");
            sheet->writeStr(1, 2, L"quality");
            sheet->writeStr(1, 3, L"icon");
            sheet->writeStr(1, 4, L"screenshot");
            sheet->writeStr(1, 5, L"jsonequip");
            sheet->writeStr(1, 6, L"attainable");
            sheet->writeStr(1, 7, L"flags2");

            //Search for the start of the database
            size_t index = page.find("WH.Gatherer.addData(3, 1, {\"");
            //Search for end of database
            size_t last_index = page.find("}];", index + 1);
            //Offset to the position of the first element
            index = index + 10;

            while (index < last_index)
            {
                index++;

                //cout << page[index] << endl;

                switch (page[index])
                {
                case '\"':
                {
                    if (quote_mark_L <= quote_mark_R)
                        quote_mark_L = index;
                    else if (quote_mark_L > quote_mark_R)
                    {
                        quote_mark_R = index;
                        cell_str = page.substr(quote_mark_L + 1, quote_mark_R - quote_mark_L - 1);
                        //cout << quote_mark_L << "     " << quote_mark_R - quote_mark_L << endl;
                        //cout << cell_str << endl;
                        if (cell_str == "name_enus")
                            table_column = 1;
                        else if (cell_str == "quality")
                            table_column = 2;
                        else if (cell_str == "icon")
                            table_column = 3;
                        else if (cell_str == "screenshot")
                            table_column = 4;
                        else if (cell_str == "jsonequip")
                        {
                            table_column = 5;
                            json_start = page.find('{', index);
                            json_end = page.find('}', index);
                            cell_str = page.substr(json_start, json_end - json_start);
                            w_cell_str = converter.from_bytes(cell_str);
                            sheet->writeStr(table_row, table_column, w_cell_str.c_str());
                            index = json_end;
                        }
                        else if (cell_str == "attainable")
                            table_column = 6;
                        else if (cell_str == "flags2")
                            table_column = 7;
                        else
                        {
                            w_cell_str = converter.from_bytes(cell_str);
                            sheet->writeStr(table_row, table_column, w_cell_str.c_str());
                            //table_column = 0;
                            //cout << cell_str << endl;
                        }
                    }
                    break;
                }
                case '{':
                {
                    bracket_count_L++;
                    break;
                }
                case '}':
                {
                    bracket_count_R++;
                    table_row++;
                    if (bracket_count_L == bracket_count_R)
                        table_column = 0;
                    break;
                }
                case ':':
                {
                    if (IsItNumb(page[index + 1]))
                    {
                        numb_str = "";
                        while (IsItNumb(page[index + 1]))
                        {
                            numb_str += page[index + 1];
                            index++;
                        }
                        sheet->writeNum(table_row, table_column, StringToNumb(numb_str));
                        //table_column = 0;
                    }
                    break;
                }
                }

            }
            cout << "1 : " << index << endl;
            cout << "2 : " << last_index << endl;
        }
        book->save(L"tmp_table.xls");
        book->release();
    }
    return true;
}

bool IsItNumb(char ch)
{
    if ((ch == '0') || (ch == '1') || (ch == '2') || (ch == '3') || (ch == '4')
        || (ch == '5') || (ch == '6') || (ch == '7') || (ch == '8') || (ch == '9'))
        return true;
    else
        return false;
}

int StringToNumb(string str)
{
    int numb = 0;
    int length = str.length();
    for (int i = 0; i < length; i++)
    {
        numb = numb + (str[i] - '0') * pow(10, length - i - 1);
    }
    return numb;
}