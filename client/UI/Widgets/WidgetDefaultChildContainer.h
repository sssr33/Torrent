#pragma once
#include "IWidget.h"

#include <vector>

template<class BaseT>
class WidgetDefaultChildContainer : public BaseT {
public:
    void AddChild(std::shared_ptr<IWidget> widget) override {
        auto thisParent = this->shared_from_this();
        auto widgetParent = widget->GetParent();

        if (thisParent == widgetParent) {
            return;
        }

        if (widgetParent != nullptr) {
            widgetParent->RemoveChild(widget);
        }

        widget->SetParent(thisParent);

        this->children.push_back(std::move(widget));
    }

    void RemoveChild(std::shared_ptr<IWidget> widget) override {
        auto thisParent = this->shared_from_this();
        auto widgetParent = widget->GetParent();

        if (thisParent != widgetParent) {
            // if some logic will require to remove assert then remove
            assert(false);
            return;
        }

        auto it = std::find(this->children.begin(), this->children.end(), widget);
        if (it != this->children.end()) {
            this->children.erase(it);
        }
        else {
            // widget has thisParent but not in children collection
            // Looks like logic error
            assert(false);
        }

        widget->SetParent({});
    }

    void DrawChildren(WidgetRenderContext& ctx) {
        for (auto& i : this->children) {
            i->Draw(ctx);
        }
    }

protected:
    std::vector<std::shared_ptr<IWidget>> children;
};
