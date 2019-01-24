// ************************************************************************** //
//
//  Prototype of mini MVVM framework for bornagainproject.org
//
//! @homepage  http://www.bornagainproject.org
//! @license   GNU General Public License v3 or higher
//
// ************************************************************************** //

#ifndef VIEWITEM_H
#define VIEWITEM_H

#include "global.h"
#include <QStandardItem>

namespace ModelView {

//! Represents the view of SessionItem's data in single cell of Qt table/tree.
//! Act as QStandardItem which stores SessionItem inside.
//!
//! In principle, it is intended to show SessionItem's data role, but it can be used for
//! any role SessionItem can carry on board.

class CORE_EXPORT ViewItem : public QStandardItem {
public:
    explicit ViewItem(const QString& text);
};

}  // namespace ModelView

#endif // VIEWITEM_H