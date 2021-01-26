// ************************************************************************** //
//
//  Model-view-view-model framework for large GUI applications
//
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @authors   see AUTHORS
//
// ************************************************************************** //

#ifndef GRAPHICSPROXYCORE_REGIONOFINTERESTVIEW_H
#define GRAPHICSPROXYCORE_REGIONOFINTERESTVIEW_H

#include <QGraphicsItem>
#include <memory>
#include <vector>

namespace ModelView {
class SceneAdapterInterface;
}

namespace GraphicsProxy {

class RegionOfInterestItem;
class RegionOfInterestController;
class SizeHandleElement;

//! Graphics object to represent RegionOfInterestItem on graphics scene.
//! Follows standard QGraphicsScene notations: (x,y) origin is top left corner.

class RegionOfInterestView : public QGraphicsItem {
public:
    RegionOfInterestView(RegionOfInterestItem* item,
                         const ModelView::SceneAdapterInterface* scene_adapter);
    ~RegionOfInterestView() override;

    QRectF boundingRect() const override;

    void advance(int phase) override;

    void setActiveHandle(SizeHandleElement* element);

    void update_geometry();

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    void create_size_handle_elements();
    SizeHandleElement* findOpposite(SizeHandleElement* element);
    std::unique_ptr<RegionOfInterestController> m_controller;
    std::vector<SizeHandleElement*> m_handles;
    SizeHandleElement* m_activeHandle{nullptr}; //!
    QPointF m_oppositeOrigin; //! coordinate of opposite corner at the moment of click
};

} // namespace GraphicsProxy

#endif // GRAPHICSPROXYCORE_REGIONOFINTERESTVIEW_H
