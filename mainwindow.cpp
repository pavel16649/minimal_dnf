#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cmath>
#include <iostream>
#include <vector>
#include <set>

using std::vector;

vector <vector <QString>> lines_of_vars;
vector <QString> letters;
std::set <QString> bruteforce;
vector <vector <QString>> allowed;
vector <vector <bool>> chosen;
vector <QString> ans;
std::set <std::set <QString>> ans_set;
int min_allowed = 10e6;

int generator_column_num = 0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Set valid symbols for dec vector lineEdit

    //Set valid symbols for bin vector lineEdit
    QRegExp exp2("[0-1]+");
    ui->lineEdit->setValidator(new QRegExpValidator(exp2, this));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
    QString bin_num = ui->lineEdit->text();

    int num_of_variables = ui->lineEdit_3->text().toInt();

    generate(bin_num, num_of_variables);
}

void gen_lines(QString pref, const int &len)
{
    if (pref.size() == len) {
        for (int i = 0; i < len; ++ i) {
            QString str(pref[i]);
            lines_of_vars[generator_column_num].push_back(str);
        }

        ++generator_column_num;

        int count_one = 0;
        for (int i = 0; i < pref.size(); ++i) {
            if (pref[i] == '1') {
                ++count_one;
            }
        }

        if (count_one <= 1) {
            return;
        }

        QString str = "";
        for (char i = 0; i < len; ++i) {
            if (pref[i] == '1') {
                str += ('a' + i);

            }
        }
        letters.push_back(str);

    } else {
        gen_lines(pref + "0", len);
        gen_lines(pref + "1", len);
    }
}

bool comparator(const QString &left, const QString &right)
{
    if (left.size() < right.size()) {
        return true;
    } else if (left.size() > right.size()) {
        return false;
    }

    return left < right;
}

void gen_the_rest()
{
    for (unsigned int i = 0; i < lines_of_vars.size(); ++i) {
        for(unsigned int j = 0; j < letters.size(); ++j) {
            QString str = "";
            QChar tmp = 'a';
            for(int k = 0; k < letters[j].size(); ++k) {
                str += lines_of_vars[i][(letters[j][k].unicode() - tmp.unicode())];
            }

            lines_of_vars[i].push_back(str);
        }
    }
}



void ban_nulls(QString num_of_func)
{
    vector <int> index;
    for (int i = 0; i < num_of_func; ++i) {
        if (num_of_func[i] == '0') {
            index.push_back(i);
        }
    }
    for (unsigned int col = 0; col < lines_of_vars[0].size(); ++col) {
        for (auto ind : index) {
            for (int row = 0; row < lines_of_vars.size(); ++row) {
                if (lines_of_vars[ind][col] == "-1") {
                    break;
                }

                if (row == ind) {
                    continue;
                }

                if (lines_of_vars[row][col] == lines_of_vars[ind][col]) {
                    lines_of_vars[row][col] = "-1";
                }
            }

            lines_of_vars[ind][col] = "-1";
        }

    }
}

void MainWindow::paint(QString num_of_func)
{
    for (unsigned int i = 0; i < lines_of_vars.size(); ++i) {
        for (unsigned int j = 0; j < lines_of_vars[i].size(); ++j) {
            if (lines_of_vars[i][j] == "-1") {
                ui->tableWidget->item(i, j)->setBackground(Qt::blue);

            } else if (lines_of_vars[i][j] == "-2") {
                ui->tableWidget->item(i, j)->setBackground(Qt::yellow);

            } else if (lines_of_vars[i][j] == "-3") {
                ui->tableWidget->item(i, j)->setBackground(Qt::darkCyan);

            } else {
                ui->tableWidget->item(i, j)->setBackground(Qt::green);
            }
        }
    }

    for (unsigned int i = 0; i < lines_of_vars.size(); ++i) {
        if (num_of_func[i] == "0"){
            for (unsigned int j = 0; j < lines_of_vars[i].size(); ++j) {
                 ui->tableWidget->item(i, j)->setBackground(Qt::red);
            }
        }
    }
}

vector <std::pair<int, int>> absorption_and_theOnly()
{
    vector <std::pair<int, int>> onlies;
    for (unsigned int i = 0; i < lines_of_vars.size(); ++i) {
        // Absorption
        int ind_first_allowed = 10e6;
        bool flag_found = false;

        for (unsigned int j = 0; j < lines_of_vars[i].size(); ++j) {
            if (lines_of_vars[i][j] != "-1") {
                ind_first_allowed = j;
                flag_found = true;
                break;
            }
        }

        if (!flag_found) {
            continue;
        }

        int count = 1;
        for (unsigned int j = ind_first_allowed + 1; j < lines_of_vars[i].size(); ++j) {
            if (lines_of_vars[i][j] != "-1" && letters[j].size() > letters[ind_first_allowed].size()) {
                lines_of_vars[i][j] = "-2";
            } else if (lines_of_vars[i][j] != "-1") {
                ++count;
            }
        }

        // The only

        if (count == 1) {
            QString str = "";
            for(int k = 0; k < letters[ind_first_allowed].size(); ++k) {
                if (lines_of_vars[i][ind_first_allowed][k] == '1') {
                    str += letters[ind_first_allowed][k];
                } else {
                    str += "!" + letters[ind_first_allowed][k];
                }
            }

            onlies.push_back({i, ind_first_allowed});
        }
    }

    return onlies;
}

void auto_choose(vector <std::pair<int, int>> &onlies)
{

    for (auto elem : onlies) {
        for (unsigned int row = 0; row < lines_of_vars.size(); ++row) {
            if (row == elem.first) {
                continue;
            }

            if (lines_of_vars[row][elem.second] == lines_of_vars[elem.first][elem.second]) {
                lines_of_vars[row][elem.second] = "-3";
                for (unsigned int col = 0; col < lines_of_vars[row].size(); ++col) {
                    if (lines_of_vars[row][col] != "-1"
                            && lines_of_vars[row][col] != "-2") {
                        lines_of_vars[row][col] = "-3";
                    }
                    if (lines_of_vars[row][col] != "-1"
                            && lines_of_vars[row][col] != "-2"
                            && lines_of_vars[row][col] != "-3") {

                        QString str = "";
                        for (int ch = 0; ch < lines_of_vars[elem.first][elem.second]; ++ch) {
                            if (lines_of_vars[elem.first][elem.second][ch] == "1") {
                                str += letters[elem.second][ch];
                            } else {
                                str += "!" + letters[elem.second][ch];
                            }
                        }

                        lines_of_vars[elem.first][elem.second] = "-4";

                        bruteforce.insert(str);
                    }
                }
            }
        }

    }
}

void search_allowed()
{
    for (unsigned int i = 0; i < lines_of_vars.size(); ++i) {
        vector <QString> tmp = {};
        allowed.push_back(tmp);
        for (unsigned int j = 0; j < lines_of_vars[i].size(); ++j) {
            if (lines_of_vars[i][j] != "-1"
                    && lines_of_vars[i][j] != "-2"
                    && lines_of_vars[i][j] != "-3"
                    && lines_of_vars[i][j] != "-4") {

                QString str = "";
                for (int ch = 0; ch < lines_of_vars[i][j].size(); ++ch) {
                    if (lines_of_vars[i][j][ch] == "1") {
                        str += letters[j][ch];
                    } else {
                        str += "!" + letters[j][ch];
                    }
                }
                (*(allowed.end() - 1)).push_back(str);
            }
        }
        if ((*(allowed.end() - 1)).size() == 0) {
            allowed.pop_back();
        }
    }

}

void bruteForce(int ind)
{
    if (ind == allowed.size()) {
        if (bruteforce.size() < min_allowed) {
            min_allowed = bruteforce.size();
        }
        if(!ans_set.count(bruteforce)) {
            ans_set.insert(bruteforce);
        }
        return;

    }
    for (unsigned int i = 0; i < allowed[ind].size(); ++i) {
        if (chosen[ind][i]) continue;
        chosen[ind][i] = true;

        bool flag = false;
        if (!bruteforce.count(allowed[ind][i])) {
            bruteforce.insert(allowed[ind][i]);
            flag = true;
        }

        bruteForce(ind + 1);

        if (flag) {
            bruteforce.erase(allowed[ind][i]);
        }

        chosen[ind][i] = false;

    }

}

void gen_ans()
{
    for (auto elem : ans_set) {
        if (elem.size() > min_allowed) continue;

        QString str1 = "";
        for (auto str : elem) {
            str1 += str + "+";
        }
        str1.remove(str1.end() - str1.begin() -1, 1);

        ans.push_back(str1);
    }


}
void MainWindow::clear()
{
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(0);
    ui->tableWidget->setRowCount(0);
    lines_of_vars.clear();
    letters.clear();
    ans.clear();
    ans_set.clear();
    bruteforce.clear();
    allowed.clear();
    min_allowed = 10e6;
    chosen.clear();
    generator_column_num = 0;
    ui->textEdit->clear();

    ui->tableWidget->setShowGrid(true);

}


void MainWindow::generate(QString num_of_func, int num_of_variables)
{
    // Generate the table
    //--------------------
    clear();

    int num_of_values = num_of_func.size();
    ui->tableWidget->setRowCount(num_of_values);

    lines_of_vars.resize(num_of_values);

    gen_lines("", num_of_variables);

    std::sort(letters.begin(), letters.end(), comparator);

    gen_the_rest();

    ui->tableWidget->setColumnCount(lines_of_vars[0].size());


    ui->tableWidget->horizontalHeader()->setVisible(true);

    int count = 0;
    for (; count < num_of_variables; ++count) {
        ui->tableWidget->setHorizontalHeaderItem(count, new QTableWidgetItem(QString('a' + count)));
    }
    for (unsigned int i = 0; i < letters.size(); ++i) {
        ui->tableWidget->setHorizontalHeaderItem(count, new QTableWidgetItem(letters[i]));
        ++count;
    }


    for (unsigned int i = 0; i < lines_of_vars.size(); ++i) {
        for (unsigned int j = 0; j < lines_of_vars[i].size(); ++j) {
            ui->tableWidget->setItem(i, j, new QTableWidgetItem(lines_of_vars[i][j]));
        }
    }


    ui->tableWidget->verticalHeader()->setVisible(true);

    for (int i = 0; i < num_of_func.size(); ++i) {
        QString str(num_of_func[i]);
        ui->tableWidget->setVerticalHeaderItem(i, new QTableWidgetItem(str));
    }


    ban_nulls(num_of_func);


    for (int i = 0; i < num_of_variables; ++i) {
        QString str('a' + i);
        letters.push_back(str);
    }

    std::sort(letters.begin(), letters.end(), comparator);


    vector <std::pair<int, int>> onlies = absorption_and_theOnly();

    if (onlies.size()) {
        auto_choose(onlies);
    }

    paint(num_of_func);

    search_allowed();


    chosen.resize(allowed.size());

    for (unsigned int i = 0; i < allowed.size(); ++i) {
        for (unsigned int j = 0; j < allowed[i].size(); ++j) {
            chosen[i].push_back(false);
        }
    }

    if (allowed.size()) {
       bruteForce(0);
    }


    gen_ans();
    for (auto elem : ans) {
        ui->textEdit->insertPlainText(elem + "\n");
    }
}
