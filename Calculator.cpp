#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSignalMapper>

class Calculator : public QWidget {
    Q_OBJECT

public:
    Calculator(QWidget *parent = 0);

private slots:
    void onDigitClicked(const QString &digit);
    void onOperatorClicked(const QString &op);
    void onClear();
    void onEquals();

private:
    QLineEdit *display;
    QString currentInput;
    double firstOperand;
    QString pendingOperator;
    bool waitingForSecondOperand;

    void calculate();
};

Calculator::Calculator(QWidget *parent) : QWidget(parent), firstOperand(0), waitingForSecondOperand(false) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    display = new QLineEdit("0");
    display->setAlignment(Qt::AlignRight);
    display->setReadOnly(true);
    display->setStyleSheet("font-size: 30px;");

    QGridLayout *gridLayout = new QGridLayout;

    QStringList digitButtons = {"7", "8", "9", "4", "5", "6", "1", "2", "3", "0"};
    QSignalMapper *digitMapper = new QSignalMapper(this);
    for (int i = 0; i < digitButtons.size(); ++i) {
        QPushButton *button = new QPushButton(digitButtons[i]);
        button->setMinimumSize(60, 60);
        button->setStyleSheet("font-size: 20px;");
        gridLayout->addWidget(button, i / 3, i % 3);
        connect(button, SIGNAL(clicked()), digitMapper, SLOT(map()));
        digitMapper->setMapping(button, digitButtons[i]);
    }
    connect(digitMapper, SIGNAL(mapped(const QString &)), this, SLOT(onDigitClicked(const QString &)));

    QPushButton *addButton = new QPushButton("+");
    QPushButton *subButton = new QPushButton("-");
    QPushButton *mulButton = new QPushButton("*");
    QPushButton *divButton = new QPushButton("/");
    QPushButton *clearButton = new QPushButton("C");
    QPushButton *equalButton = new QPushButton("=");

    QList<QPushButton *> operators = {addButton, subButton, mulButton, divButton};

    QSignalMapper *opMapper = new QSignalMapper(this);
    for (QPushButton *opButton : operators) {
        opButton->setMinimumSize(60, 60);
        opButton->setStyleSheet("font-size: 20px;");
        connect(opButton, SIGNAL(clicked()), opMapper, SLOT(map()));
        opMapper->setMapping(opButton, opButton->text());
    }
    connect(opMapper, SIGNAL(mapped(const QString &)), this, SLOT(onOperatorClicked(const QString &)));

    connect(clearButton, &QPushButton::clicked, this, &Calculator::onClear);
    connect(equalButton, &QPushButton::clicked, this, &Calculator::onEquals);

    gridLayout->addWidget(addButton, 0, 3);
    gridLayout->addWidget(subButton, 1, 3);
    gridLayout->addWidget(mulButton, 2, 3);
    gridLayout->addWidget(divButton, 3, 3);
    gridLayout->addWidget(clearButton, 3, 0);
    gridLayout->addWidget(equalButton, 3, 2);

    mainLayout->addWidget(display);
    mainLayout->addLayout(gridLayout);

    setLayout(mainLayout);
    setWindowTitle("Calculator");
}

void Calculator::onDigitClicked(const QString &digit) {
    if (waitingForSecondOperand) {
        display->clear();
        waitingForSecondOperand = false;
    }

    if (display->text() == "0") {
        display->setText(digit);
    } else {
        display->setText(display->text() + digit);
    }

    currentInput = display->text();
}

void Calculator::onOperatorClicked(const QString &op) {
    if (!waitingForSecondOperand) {
        firstOperand = currentInput.toDouble();
        pendingOperator = op;
        waitingForSecondOperand = true;
    }
}

void Calculator::onClear() {
    display->setText("0");
    currentInput.clear();
    firstOperand = 0;
    waitingForSecondOperand = false;
}

void Calculator::onEquals() {
    double secondOperand = currentInput.toDouble();
    if (!pendingOperator.isEmpty()) {
        if (pendingOperator == "+") {
            firstOperand += secondOperand;
        } else if (pendingOperator == "-") {
            firstOperand -= secondOperand;
        } else if (pendingOperator == "*") {
            firstOperand *= secondOperand;
        } else if (pendingOperator == "/") {
            if (secondOperand != 0) {
                firstOperand /= secondOperand;
            } else {
                display->setText("Error");
                return;
            }
        }
    }

    display->setText(QString::number(firstOperand));
    waitingForSecondOperand = true;
    currentInput = QString::number(firstOperand);
    pendingOperator.clear();
}

void Calculator::calculate() {
    // Perform calculations if needed, optional for advanced logic
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    Calculator calculator;
    calculator.show();

    return app.exec();
}
