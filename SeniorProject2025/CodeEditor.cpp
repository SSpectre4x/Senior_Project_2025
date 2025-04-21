#include "CodeEditor.h"

#include <QPainter>
#include <QTextBlock>
#include <QToolTip>

CodeEditor::CodeEditor(QWidget* parent) : QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);

    updateLineNumberAreaWidth(0);
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int newBlockCount)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect& rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent* e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent* event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

bool CodeEditor::event(QEvent* event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);

        QPoint pos = helpEvent->pos();
        pos.setX(pos.x() - viewportMargins().left());
        pos.setY(pos.y() - viewportMargins().top());

        QTextCursor cursor = cursorForPosition(pos);
        cursor.select(QTextCursor::LineUnderCursor);

        if (!cursor.selectedText().isEmpty())
        {
            QString tooltip;
            int line = cursor.blockNumber() + 1;
            if (!lineErrors[line].empty())
            {
                for (int i = 0; i > lineErrors[line].size() - 1; i++)
                {
                    tooltip += QString::fromStdString(lineErrors[line].at(i).str());
                    tooltip += "\n\n";
                }
                tooltip += QString::fromStdString(lineErrors[line].at(lineErrors[line].size() - 1).str());
            }    
            if (!tooltip.isEmpty())
                QToolTip::showText(helpEvent->globalPos(), tooltip);
        }

        else
            QToolTip::hideText();
        return true;
    }
    return QPlainTextEdit::event(event);
}