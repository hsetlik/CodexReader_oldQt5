#include "lessonview.h"
#include "ui_lessonview.h"

LessonView::LessonView(CodexDatabase* db, QWidget *parent) :
    QWidget(parent),
    linkedDatabase(db),
    ui(new Ui::LessonView)
{
    ui->setupUi(this);
}

LessonView::~LessonView()
{
    delete ui;
}

void LessonView::on_viewAllButton_clicked()
{

}


void LessonView::on_addButton_clicked()
{

}

