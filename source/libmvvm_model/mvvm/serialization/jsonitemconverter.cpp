// ************************************************************************** //
//
//  Model-view-view-model framework for large GUI applications
//
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @authors   see AUTHORS
//
// ************************************************************************** //

#include <QJsonArray>
#include <QJsonObject>
#include <mvvm/core/uniqueidgenerator.h>
#include <mvvm/interfaces/itemfactoryinterface.h>
#include <mvvm/model/sessionitem.h>
#include <mvvm/model/sessionitemdata.h>
#include <mvvm/model/sessionitemtags.h>
#include <mvvm/serialization/jsonitem_types.h>
#include <mvvm/serialization/jsonitemconverter.h>
#include <mvvm/serialization/jsonitemdataconverter.h>
#include <mvvm/serialization/jsonitemformatassistant.h>
#include <mvvm/serialization/jsonitemtagsconverter.h>

using namespace ModelView;

namespace
{

//! Returns true if flags requires generation of new ID instead of using the one stored in JSON.

bool isRegenerateID(const ConverterFlags& flags)
{
    return !hasFlag(flags, ConverterFlags::USE_JSON_ID);
}

//! Returns true if flags requires reset of item data and tags.

bool isResetItemDataAndTags(const ConverterFlags& flags)
{
    return hasFlag(flags, ConverterFlags::COPY_JSON_DATA)
           && hasFlag(flags, ConverterFlags::COPY_JSON_TAGS);
}

//! Creates converter for SessionItemData/JSON.

std::unique_ptr<JsonItemDataConverter> createDataConverter(const ConverterFlags& flags)
{
    if (flags == ConverterFlags::PROJECT_MODE) {
        // PROJECT_MODE assumes that we are serializing only certain roles
        auto accept_roles = [](auto role) {
            return role == ItemDataRole::IDENTIFIER || role == ItemDataRole::DATA;
        };
        return std::make_unique<JsonItemDataConverter>(accept_roles, accept_roles);
    } else {
        return std::make_unique<JsonItemDataConverter>();
    }
}

} // namespace

struct JsonItemConverter::JsonItemConverterImpl {
    JsonItemConverter* m_parent{nullptr};
    std::unique_ptr<JsonItemDataConverter> m_itemdata_converter;
    std::unique_ptr<JsonItemTagsConverter> m_itemtags_converter;
    ConverterContext m_context;

    JsonItemConverterImpl(JsonItemConverter* parent, const ConverterContext& context)
        : m_parent(parent), m_context(context)
    {
        //! Callback to convert SessionItem to JSON object.
        auto create_json = [this](const SessionItem& item) { return m_parent->to_json(&item); };

        //! Callback to create SessionItem from JSON object.
        auto create_item = [this](const QJsonObject& json) { return m_parent->from_json(json); };

        //! Callback to update SessionItem from JSON object.
        auto update_item = [this](const QJsonObject& json, SessionItem* item) {
            populate_item(json, *item);
        };

        ConverterCallbacks callbacks{create_json, create_item, update_item};

        m_itemdata_converter = createDataConverter(m_context.m_flags);
        m_itemtags_converter = std::make_unique<JsonItemTagsConverter>(callbacks);
    }

    const ItemFactoryInterface* factory() { return m_context.m_factory; }

    void populate_item_data(const QJsonArray& json, SessionItemData& item_data)
    {
        m_itemdata_converter->from_json(json, item_data);
    }

    void populate_item_tags(const QJsonObject& json, SessionItemTags& item_tags)
    {
        m_itemtags_converter->from_json(json, item_tags);
    }

    void populate_item(const QJsonObject& json, SessionItem& item)
    {
        auto modelType = json[JsonItemFormatAssistant::modelKey].toString().toStdString();

        if (modelType != item.modelType())
            throw std::runtime_error("Item model mismatch");

        if (isResetItemDataAndTags(m_context.m_flags))
            item.setDataAndTags(std::make_unique<SessionItemData>(),
                                std::make_unique<SessionItemTags>());

        populate_item_data(json[JsonItemFormatAssistant::itemDataKey].toArray(), *item.itemData());
        populate_item_tags(json[JsonItemFormatAssistant::itemTagsKey].toObject(), *item.itemTags());

        for (auto child : item.children())
            child->setParent(&item);

        if (isRegenerateID(m_context.m_flags))
            item.setData(UniqueIdGenerator::generate(), ItemDataRole::IDENTIFIER);
    }

    QJsonObject item_to_json(const SessionItem& item) const
    {
        QJsonObject result;
        result[JsonItemFormatAssistant::modelKey] = QString::fromStdString(item.modelType());
        result[JsonItemFormatAssistant::itemDataKey] =
            m_itemdata_converter->to_json(*item.itemData());
        result[JsonItemFormatAssistant::itemTagsKey] =
            m_itemtags_converter->to_json(*item.itemTags());

        return result;
    }
};

JsonItemConverter::JsonItemConverter(const ConverterContext& context)
    : p_impl(std::make_unique<JsonItemConverterImpl>(this, context))
{
}

JsonItemConverter::~JsonItemConverter() = default;

QJsonObject JsonItemConverter::to_json(const SessionItem* item) const
{
    return item ? p_impl->item_to_json(*item) : QJsonObject();
}

std::unique_ptr<SessionItem> JsonItemConverter::from_json(const QJsonObject& json) const
{
    static JsonItemFormatAssistant assistant;

    if (!assistant.isSessionItem(json))
        throw std::runtime_error("JsonItemConverterV2::from_json() -> Error. Given json object "
                                 "can't represent a SessionItem.");

    auto modelType = json[JsonItemFormatAssistant::modelKey].toString().toStdString();
    auto result = p_impl->factory()->createItem(modelType);

    p_impl->populate_item(json, *result);

    return result;
}
