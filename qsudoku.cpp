#include <QMessageBox>

#include "qsudoku.h"
#include "./ui_qsudoku.h"

enum class CheckResultStatus
{
    InProgress,
    Done,
    Error
};

struct CheckResult
{
    CheckResultStatus status;
    QList<uint8_t> badNums;
};

QSudoku::QSudoku(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::QSudoku), m_grid({0})
{
    const QList<QComboBox *> initVector(GRID_SIZE, nullptr);
    m_fields = QList<QList<QComboBox *>>(GRID_SIZE, initVector);
    ui->setupUi(this);

    const QStringList ITEMS = {
        "", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
    for (uint64_t i = 0; i < GRID_SIZE; ++i)
    {
        for (uint64_t j = 0; j < GRID_SIZE; ++j)
        {
            QComboBox *cell = new QComboBox(this);
            cell->addItems(ITEMS);
            ui->gridLayout->addWidget(cell, i + i / 3, j + j / 3);
            m_fields[i][j] = cell;
            connect(cell, &QComboBox::currentTextChanged, [this, cell, i, j](const QString &newText)
                    {
                        uint8_t digit = newText.toUShort();
                        m_grid[i*GRID_SIZE+j] = digit;
                        checkForWin(); });
        }
    }

    for (uint64_t i = 0; i < 3; i++)
    {
        for (uint64_t j = 0; j < 2; j++)
        {
            QFrame *hFrame = new QFrame(this);
            hFrame->setFrameShape(QFrame::HLine);
            ui->gridLayout->addWidget(hFrame, 4 * j + 3, 4 * i, 1, 3);

            QFrame *vFrame = new QFrame(this);
            vFrame->setFrameShape(QFrame::VLine);
            ui->gridLayout->addWidget(vFrame, 4 * i, 4 * j + 3, 3, 1);
        }
    }
}

QSudoku::~QSudoku()
{
    delete ui;
}

void QSudoku::checkForWin()
{
    uint64_t goodCols = 0;
    uint64_t goodRows = 0;
    uint64_t goodBoxes = 0;

    // Clear all background colors of cells
    for (auto row : m_fields)
    {
        for (auto cell : row)
        {
            cell->setStyleSheet("");
        }
    }

    auto checkNums = [this](const uint8_t *nums)
    {
        uint64_t goodNums = 0;
        QList<uint8_t> badNums;
        for (uint64_t digit = 1; digit < 10; ++digit)
        {
            uint8_t count = nums[digit - 1];
            if (count == 1)
            {
                ++goodNums;
            }
            else if (count > 1)
            {
                badNums.append(digit);
            }
        }
        CheckResultStatus status = CheckResultStatus::InProgress;
        if (!badNums.isEmpty())
        {
            status = CheckResultStatus::Error;
        }
        else if (goodNums == 9)
        {
            status = CheckResultStatus::Done;
        }
        return CheckResult{status, badNums};
    };

    // Check columns
    for (uint64_t col = 0; col < GRID_SIZE; ++col)
    {
        uint8_t nums[GRID_SIZE] = {0};
        for (uint64_t row = 0; row < GRID_SIZE; ++row)
        {
            uint8_t digit = m_grid[row * GRID_SIZE + col];
            if (digit != 0)
            {
                nums[digit - 1] += 1;
            }
        }

        auto result = checkNums(nums);
        switch (result.status)
        {
        case CheckResultStatus::Done:
            ++goodCols;
            break;
        case CheckResultStatus::Error:
            for (auto num : result.badNums)
            {
                for (uint64_t row = 0; row < GRID_SIZE; ++row)
                {
                    uint8_t digit = m_grid[row * GRID_SIZE + col];
                    if (digit == num)
                    {
                        m_fields[row][col]->setStyleSheet("background-color: red");
                    }
                }
            }
            break;
        default:
            // Nothing to do
            break;
        }
    }
    // Check rows
    for (uint64_t row = 0; row < GRID_SIZE; ++row)
    {
        uint8_t nums[GRID_SIZE] = {0};
        for (uint64_t col = 0; col < GRID_SIZE; ++col)
        {
            uint8_t digit = m_grid[row * GRID_SIZE + col];
            if (digit != 0)
            {
                nums[digit - 1] += 1;
            }
        }

        auto result = checkNums(nums);
        switch (result.status)
        {
        case CheckResultStatus::Done:
            ++goodRows;
            break;
        case CheckResultStatus::Error:
            for (auto num : result.badNums)
            {
                for (uint64_t col = 0; col < GRID_SIZE; ++col)
                {
                    uint8_t digit = m_grid[row * GRID_SIZE + col];
                    if (digit == num)
                    {
                        m_fields[row][col]->setStyleSheet("background-color: red");
                    }
                }
            }
            break;
        default:
            // Nothing to do
            break;
        }
    }
    // Check boxes
    for (uint64_t boxRow = 0; boxRow < GRID_SIZE / 3; ++boxRow)
    {
        for (uint64_t boxCol = 0; boxCol < GRID_SIZE / 3; ++boxCol)
        {

            uint8_t nums[GRID_SIZE] = {0};
            for (uint64_t row = 0; row < GRID_SIZE / 3; ++row)
            {
                for (uint64_t col = 0; col < GRID_SIZE / 3; ++col)
                {
                    uint64_t realRow = boxRow * 3 + row;
                    uint64_t realCol = boxCol * 3 + col;
                    uint8_t digit = m_grid[realRow * GRID_SIZE + realCol];
                    if (digit != 0)
                    {
                        nums[digit - 1] += 1;
                    }
                }
            }

            auto result = checkNums(nums);
            switch (result.status)
            {
            case CheckResultStatus::Done:
                ++goodBoxes;
                break;
            case CheckResultStatus::Error:
                for (auto num : result.badNums)
                {
                    for (uint64_t row = 0; row < GRID_SIZE / 3; ++row)
                    {
                        for (uint64_t col = 0; col < GRID_SIZE / 3; ++col)
                        {
                            uint64_t realRow = boxRow * 3 + row;
                            uint64_t realCol = boxCol * 3 + col;
                            uint8_t digit = m_grid[realRow * GRID_SIZE + realCol];
                            if (digit == num)
                            {
                                m_fields[realRow][realCol]->setStyleSheet("background-color: red");
                            }
                        }
                    }
                }
                break;
            default:
                // Nothing to do
                break;
            }
        }
    }

    if (goodCols == 9 && goodRows == 9 && goodBoxes == 9)
    {
        for (auto row : m_fields)
        {
            for (auto cell : row)
            {
                cell->setStyleSheet("background-color: green");
            }
        }
        QMessageBox::information(this, "Win!", "You Win!");
    }
}

void QSudoku::errorDetected()
{
    QMessageBox::warning(this, "Contradiction!", "A Contradition was detected!");
}
