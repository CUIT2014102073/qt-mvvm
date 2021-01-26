// ************************************************************************** //
//
//  Model-view-view-model framework for large GUI applications
//
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @authors   see AUTHORS
//
// ************************************************************************** //

#ifndef GRAPHICSPROXYCORE_SCENEMODEL_H
#define GRAPHICSPROXYCORE_SCENEMODEL_H

#include "mvvm/model/sessionmodel.h"

namespace ModelView {
class ContainerItem;
}

namespace GraphicsProxy {

//! Main application model with data for graphics scene.

class SceneModel : public ModelView::SessionModel {
public:
    SceneModel();

    void update_data(double scale);

private:
    void create_roi();
    void create_data();
    void create_colormap();
};

} // namespace GraphicsProxy

#endif // GRAPHICSPROXYCORE_SCENEMODEL_H
