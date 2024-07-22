#ifndef QSUDOKU_H
#define QSUDOKU_H

#include <QMainWindow>
#include <QComboBox>
#include <QList>

constexpr uint64_t GRID_SIZE = 9;
constexpr uint64_t TOTAL_CELLS = GRID_SIZE * GRID_SIZE;

QT_BEGIN_NAMESPACE
namespace Ui
{
    class QSudoku;
}
QT_END_NAMESPACE

class QSudoku : public QMainWindow
{
    Q_OBJECT

public:
    QSudoku(QWidget *parent = nullptr);
    ~QSudoku();

private:
    void checkForWin();
    void errorDetected();

    Ui::QSudoku *ui;
    QList<QList<QComboBox *>> m_fields;
    std::array<uint8_t, TOTAL_CELLS> m_grid;
};
#endif // QSUDOKU_H
