#pragma once
#include "WidgetBase.h"
#include "WidgetDefaultChildContainer.h"
#include "WidgetAttachedPropertyAccessor.h"

#include <vector>
#include <cstdint>

using GridWidgetBase = WidgetDefaultChildContainer<WidgetBase>;

class GridWidget final : public GridWidgetBase {
public:
    enum class SizeType {
        Pixel,
        Weighted
    };

    struct RowDefinition {
        float height = 0.f;
        SizeType sizeType = SizeType::Pixel;
    };

    struct ColumnDefinition {
        float width = 0.f;
        SizeType sizeType = SizeType::Pixel;
    };

    void Draw(WidgetRenderContext& ctx) override;

    std::optional<D2D1_RECT_F> GetChildLayoutRect(IWidget* child) override;

    void UpdateLayout();

    void SetRows(std::vector<RowDefinition> rows);
    void SetColumns(std::vector<ColumnDefinition> columns);

    static uint32_t GetWidgetGridRowIdx(IWidget& widget);
    static void SetWidgetGridRowIdx(IWidget& widget, uint32_t gridRowIdx);

    static uint32_t GetWidgetGridColumnIdx(IWidget& widget);
    static void SetWidgetGridColumnIdx(IWidget& widget, uint32_t gridColumnIdx);

private:
    struct GridCell {
        D2D1_RECT_F rect = {};
        size_t rowIdx = 0;
        size_t columnIdx = 0;

        std::vector<std::shared_ptr<IWidget>> widgets;
    };

    std::vector<GridCell> GetCells();
    float GetRequiredPixelWidth() const;
    float GetRequiredPixelHeight() const;

    static const WidgetAttachedPropertyAccessor<uint32_t> GridRowPropAccessor;
    static const WidgetAttachedPropertyAccessor<uint32_t> GridColumnPropAccessor;

    std::vector<RowDefinition> rows;
    std::vector<ColumnDefinition> columns;
    std::vector<GridCell> lastCells;
};
