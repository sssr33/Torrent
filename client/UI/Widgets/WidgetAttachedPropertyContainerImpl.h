#pragma once
#include "IWidget.h"

#include <map>

//template<typename BaseT>
//class WidgetAttachedPropertyContainerImpl : public BaseT {
//public:
//    WidgetAttachedPropertyValue GetAttachedProperty(const std::string& name) override {
//        auto it = this->props.find(name);
//        if (it != std::end(this->props)) {
//            return it->second;
//        }
//
//        return std::monostate{};
//    }
//
//    void SetAttachedProperty(const std::string& name, WidgetAttachedPropertyValue value) override {
//        this->props[name] = std::move(value);
//    }
//
//protected:
//    std::map<std::string, WidgetAttachedPropertyValue> props;
//};
