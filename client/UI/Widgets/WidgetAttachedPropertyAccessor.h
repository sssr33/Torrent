#pragma once
#include "IWidget.h"

#include <optional>
#include <string>

template<class PropT>
class WidgetAttachedPropertyAccessor {
public:
    WidgetAttachedPropertyAccessor(std::string propName)
        : propName(std::move(propName))
    {}

    std::optional<PropT> Get(IWidget& widget) const {
        auto propVal = widget.GetAttachedProperty(this->propName);

        if (auto val = std::get_if<PropT>(&propVal)) {
            return *val;
        }

        return {};
    }

    void Set(IWidget& widget, const PropT& val) const {
        widget.SetAttachedProperty(this->propName, val);
    }

private:
    std::string propName;
};
