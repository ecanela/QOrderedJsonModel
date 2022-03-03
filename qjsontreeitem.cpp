#include "qjsontreeitem.h"
#include <iostream>

using json = nlohmann::ordered_json;


// this are detail implementation of valueTypeName()
// must be updated when JsonValueType values are modified
NLOHMANN_JSON_SERIALIZE_ENUM( JsonValueType, {
                                  {Undefined, "Undefined"},
                                  {Null     , "Null"},
                                  {Boolean  , "Boolean"},
                                  {Numeric  , "Numeric"},
                                  {String   , "String"},
                                  {Array    , "Array"},
                                  {Object   , "Object"},
                                  {Binary   , "Binary"},

                              });

QJsonTreeItem::QJsonTreeItem(QJsonTreeItem *parentItem) :
    m_parentItem(parentItem)
{

}

QJsonTreeItem::QJsonTreeItem(const QString &key, const QVariant &value, JsonValueType type,
                             QJsonTreeItem *parentItem) :
    m_key(key), m_value(value), m_valueType(type), m_parentItem(parentItem)
{

}

QJsonTreeItem::~QJsonTreeItem()
{
    qDeleteAll(m_childItems);
}


void QJsonTreeItem::appendChild(QJsonTreeItem *item)
{
    m_childItems.append(item);
}

QJsonTreeItem *QJsonTreeItem::child(int row)
{
    if (row < 0 || row >= m_childItems.size())
        return nullptr;

    return m_childItems.at(row);
}

int QJsonTreeItem::childCount() const
{
    return m_childItems.count();
}

int QJsonTreeItem::columnCount() const
{
    return  2; // key and value
}

QVariant QJsonTreeItem::data(int column) const
{
    if (column < 0 || column > 2)
        return QVariant();

    return column == 0 ? m_key : m_value;
}

QJsonTreeItem * QJsonTreeItem::parentItem() const
{
    return m_parentItem;
}

int QJsonTreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<QJsonTreeItem*>(this));

    return 0;
}

void QJsonTreeItem::setData(const QString &key, const QVariant &value, JsonValueType type)
{
    m_key = key;
    m_value = value;
    m_valueType = type;
}

const QString &QJsonTreeItem::key() const
{
    return m_key;
}

const QVariant &QJsonTreeItem::value() const
{
    return m_value;
}

JsonValueType QJsonTreeItem::valueType() const
{
    return m_valueType;
}

const QString QJsonTreeItem::valueTypeName() const
{
    json j = m_valueType;
    return QString::fromStdString(j);
}

void QJsonTreeItem::setKey(const QString &newKey)
{
    m_key = newKey;
}
