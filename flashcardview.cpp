#include "flashcardview.h"
#include "ui_flashcardview.h"
#include <QKeyEvent>

FlashCardView::FlashCardView(CodexDatabase *db, QWidget *parent) :
    QWidget(parent),
    linkedDatabase(db),
    ui(new Ui::FlashCardView)
{
    ui->setupUi(this);
    linkedDatabase->termsDueNow(termsToReview);
    printf("%d terms due\n", (int)termsToReview.size());
    toNextTerm();
}

FlashCardView::~FlashCardView()
{
    delete ui;
}

void FlashCardView::toNextTerm() // returns true if 1 or more due terms left
{
   // setFocus(Qt::ActiveWindowFocusReason);
    if (termsToReview.begin() == termsToReview.end())
    {
        emit finishStudying();
        return;
    }
    else
    {
        viewer.reset(ViewerFactory::viewerFor(&termsToReview.front(), this));
        ui->viewerLayout->addWidget(viewer.get());
    }
}
void FlashCardView::keyPressEvent(QKeyEvent* event)
{
    auto key = event->key();
    if (key == Qt::Key_1)
    {
         ui->ans1->click();
    }
    else if (key == Qt::Key_2)
    {
        ui->ans2->click();
    }
    else if (key == Qt::Key_3)
    {
        ui->ans3->click();
    }
    else if (key == Qt::Key_4)
    {
        ui->ans4->click();
    }
    else if (key == Qt::Key_5)
    {
        ui->ans5->click();
    }
    else if (key == Qt::Key_Enter || key == Qt::Key_Return)
        flipCurrentCard();
}

void FlashCardView::on_ans1_clicked()
{
    answerCurrentTerm(1);
}
void FlashCardView::on_ans2_clicked()
{
    answerCurrentTerm(2);

}
void FlashCardView::on_ans3_clicked()
{
    answerCurrentTerm(3);
}
void FlashCardView::on_ans4_clicked()
{
    answerCurrentTerm(4);
}
void FlashCardView::on_ans5_clicked()
{
    answerCurrentTerm(5);
}
void FlashCardView::answerCurrentTerm(int grade)
{
    termsToReview.front().answerSM2(grade);
    linkedDatabase->updateTerm(&termsToReview.front());
    //show the card again if the grade is too low
    if (grade < 2)
    {
        auto currentFront = termsToReview.front();
        termsToReview.push_back(currentFront);
    }
    if(termsToReview.size() > 0)
        termsToReview.erase(termsToReview.begin());
    toNextTerm();
}
void FlashCardView::setButtonsVisible(bool visible)
{
    ui->ans1->setVisible(visible);
    ui->ans2->setVisible(visible);
    ui->ans3->setVisible(visible);
    ui->ans4->setVisible(visible);
    ui->ans5->setVisible(visible);
}
void FlashCardView::flipCurrentCard()
{
    viewer->flip();

}




