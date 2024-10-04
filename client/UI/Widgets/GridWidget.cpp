#include "GridWidget.h"
#include "WidgetLayout.h"

#include <algorithm>

void GridWidget::Draw(WidgetRenderContext& ctx) {
    this->UpdateLayout();

    GridWidgetBase::DrawChildren(ctx);
}

std::optional<D2D1_RECT_F> GridWidget::GetChildLayoutRect(IWidget* child) {
    auto itCell = std::find_if(this->lastCells.begin(), this->lastCells.end(),
        [child](const GridCell& cell)
        {
            auto itWidget = std::find_if(cell.widgets.begin(), cell.widgets.end(),
                [child](const std::shared_ptr<IWidget>& widget)
                {
                    return widget.get() == child;
                });

            return itWidget != cell.widgets.end();
        });

    if (itCell != this->lastCells.end()) {
        return itCell->rect;
    }

    return {};
}

void GridWidget::UpdateLayout() {
    this->lastCells = this->GetCells();
}

void GridWidget::SetRows(std::vector<RowDefinition> rows) {
    this->rows = std::move(rows);
}

void GridWidget::SetColumns(std::vector<ColumnDefinition> columns) {
    this->columns = std::move(columns);
}

uint32_t GridWidget::GetWidgetGridRowIdx(IWidget& widget) {
    if (auto rowIdx = GridWidget::GridRowPropAccessor.Get(widget)) {
        return *rowIdx;
    }

    return 0;
}

void GridWidget::SetWidgetGridRowIdx(IWidget& widget, uint32_t gridRowIdx) {
    GridWidget::GridRowPropAccessor.Set(widget, gridRowIdx);
}

uint32_t GridWidget::GetWidgetGridColumnIdx(IWidget& widget) {
    if (auto columnIdx = GridWidget::GridColumnPropAccessor.Get(widget)) {
        return *columnIdx;
    }

    return 0;
}

void GridWidget::SetWidgetGridColumnIdx(IWidget& widget, uint32_t gridColumnIdx) {
    GridWidget::GridColumnPropAccessor.Set(widget, gridColumnIdx);
}

std::vector<GridWidget::GridCell> GridWidget::GetCells() {
    auto actualRect = this->GetActualRect();
    if (!actualRect) {
        return {};
    }

    std::vector<GridCell> cells;
    cells.reserve(this->rows.size() + this->columns.size());

    auto size = D2D1::SizeF(
        actualRect->right - actualRect->left,
        actualRect->bottom - actualRect->top
    );

    auto leftTop = D2D1::Point2F(
        actualRect->left,
        actualRect->top
    );

    const float reqPixelWidth = (std::min)(this->GetRequiredPixelWidth(), size.width);
    const float reqPixelHeight = (std::min)(this->GetRequiredPixelHeight(), size.height);

    const float availWeightedWidth = size.width - reqPixelWidth;
    const float availWeightedHeight = size.height - reqPixelHeight;

    assert(availWeightedWidth >= 0.f);
    assert(availWeightedHeight >= 0.f);

    auto cellLeftTop = leftTop;

    auto normRows = this->rows;
    {
        float weightSum = 0.f;

        for (auto& i : normRows) {
            if (i.sizeType == SizeType::Weighted) {
                weightSum += i.height;
            }
        }

        if (weightSum > 0.f) {
            for (auto& i : normRows) {
                if (i.sizeType == SizeType::Weighted) {
                    i.height /= weightSum;
                }
            }
        }
    }

    auto normCols = this->columns;
    {
        float weightSum = 0.f;

        for (auto& i : normCols) {
            if (i.sizeType == SizeType::Weighted) {
                weightSum += i.width;
            }
        }

        if (weightSum > 0.f) {
            for (auto& i : normCols) {
                if (i.sizeType == SizeType::Weighted) {
                    i.width /= weightSum;
                }
            }
        }
    }

    for (size_t rowIdx = 0; rowIdx < normRows.size(); ++rowIdx) {
        auto& row = normRows[rowIdx];
        float top = cellLeftTop.y;
        float bottom = top;

        switch (row.sizeType) {
        case SizeType::Pixel:
            bottom += row.height;
            break;
        case SizeType::Weighted:
            bottom += row.height * availWeightedHeight;
            break;
        default:
            assert(false);
            break;
        }

        float left = cellLeftTop.x;

        for (size_t colIdx = 0; colIdx < normCols.size(); ++colIdx) {
            auto& col = normCols[colIdx];
            float right = left;

            switch (col.sizeType) {
            case SizeType::Pixel:
                right += col.width;
                break;
            case SizeType::Weighted:
                right += col.width * availWeightedWidth;
                break;
            default:
                assert(false);
                break;
            }

            GridCell gridCell;

            gridCell.rowIdx = rowIdx;
            gridCell.columnIdx = colIdx;

            gridCell.rect.left = left;
            gridCell.rect.right = right;
            gridCell.rect.top = top;
            gridCell.rect.bottom = bottom;

            for (const auto& widget : this->children) {
                auto widgetRowIdx = GridWidget::GetWidgetGridRowIdx(*widget);
                auto widgetColumnIdx = GridWidget::GetWidgetGridColumnIdx(*widget);

                if (gridCell.rowIdx == widgetRowIdx && gridCell.columnIdx == widgetColumnIdx) {
                    gridCell.widgets.push_back(widget);
                }
            }

            cells.push_back(std::move(gridCell));

            left = right;
        }

        cellLeftTop.y = bottom;
    }

    return cells;
}

float GridWidget::GetRequiredPixelWidth() const {
    float width = 0.f;

    for (auto& col : this->columns) {
        if (col.sizeType == SizeType::Pixel) {
            width += col.width;
        }
    }

    return width;
}

float GridWidget::GetRequiredPixelHeight() const {
    float height = 0.f;

    for (auto& row : this->rows) {
        if (row.sizeType == SizeType::Pixel) {
            height += row.height;
        }
    }

    return height;
}

const WidgetAttachedPropertyAccessor<uint32_t> GridWidget::GridRowPropAccessor("Grid.Row");
const WidgetAttachedPropertyAccessor<uint32_t> GridWidget::GridColumnPropAccessor("Grid.Column");
