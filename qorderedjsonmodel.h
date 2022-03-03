#ifndef QORDEREDJSONMODEL_H
#define QORDEREDJSONMODEL_H

#include <QAbstractItemModel>
#include <QVariant>
#include <QModelIndex>
#include <QFile>

#include "json.hpp"
#include "qjsontreeitem.h"


using json = nlohmann::ordered_json;

class QOrderedJsonModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit QOrderedJsonModel(QObject *parent = nullptr);
    explicit QOrderedJsonModel(const QString json_input, QObject *parent = nullptr);
    ~QOrderedJsonModel();

    //read only model!!
    QVariant  data(const QModelIndex &index, int role) const override ;
    Qt::ItemFlags flags(const QModelIndex &index) const override ;
    QVariant    headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    void load(const QString & jsonInput);
    bool loadFile(const QString & filepath);
    bool IsValidJson(const QString & jsonInput);

private:
    QVariant fromJsonValue(json::iterator &json);
    JsonValueType fromJsonValueType(json::iterator &json);

    QJsonTreeItem * parse_json_object(QString key, json& json_value, QJsonTreeItem *parent);
    QJsonTreeItem * parse_json_array (QString key, json& json_value, QJsonTreeItem *parent);

    QJsonTreeItem * m_rootItem;
    QStringList m_headers;
};

#endif // QORDEREDJSONMODEL_H
