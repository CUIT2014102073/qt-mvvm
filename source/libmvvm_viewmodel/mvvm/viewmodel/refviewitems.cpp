// ************************************************************************** //
//
//  Model-view-view-model framework for large GUI applications
//
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @authors   see AUTHORS
//
// ************************************************************************** //

#include <mvvm/model/sessionitem.h>
#include <mvvm/viewmodel/refviewitems.h>

using namespace ModelView;

RefRootViewItem::RefRootViewItem(SessionItem* item) : RefViewItem(item, ItemDataRole::DATA) {}

//! ---------------------------------------------------------------------------

RefViewLabelItem::RefViewLabelItem(SessionItem* item) : RefViewItem(item, ItemDataRole::DISPLAY) {}

QVariant RefViewLabelItem::data(int role) const
{
    if (!item())
        return QVariant();

    // use item's display role
    if (role == Qt::DisplayRole || role == Qt::EditRole)
        return QString::fromStdString(item()->displayName());

    return RefViewItem::data(role);
}

//! ---------------------------------------------------------------------------

RefViewDataItem::RefViewDataItem(SessionItem* item) : RefViewItem(item, ItemDataRole::DATA) {}

Qt::ItemFlags RefViewDataItem::flags() const
{
    Qt::ItemFlags result = RefViewItem::flags();
    if (item() && item()->isEditable() && item()->isEnabled())
        result |= Qt::ItemIsEditable;
    return result;
}
