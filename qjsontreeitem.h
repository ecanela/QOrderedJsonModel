#ifndef QJSONTREEITEM_H
#define QJSONTREEITEM_H

#include <QString>
#include <QVariant>
#include "json.hpp"

enum JsonValueType{
    Undefined,
    Null,
    Boolean,
    Numeric,
    String,
    Array,
    Object,
    Binary,
};


class QJsonTreeItem
{
public:

    QJsonTreeItem(QJsonTreeItem *parentItem = nullptr);
    QJsonTreeItem(const QString &key, const QVariant &value,
                  JsonValueType type,
                  QJsonTreeItem *parentItem = nullptr);
    ~QJsonTreeItem();


    QJsonTreeItem *parentItem() const;
    QJsonTreeItem *child(int row);
    void appendChild(QJsonTreeItem *child);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;

    //getters and setters
    void setData(const QString &key, const QVariant &value, JsonValueType type);
    void setKey(const QString &newKey);
    const QString &key() const;
    const QVariant &value() const;
    JsonValueType valueType() const;
    const QString valueTypeName() const;


private:
    QJsonTreeItem *        m_parentItem = nullptr;
    QList<QJsonTreeItem *> m_childItems;

    QString       m_key;
    QVariant      m_value;
    JsonValueType m_valueType;
};


#endif // QJSONTREEITEM_H

