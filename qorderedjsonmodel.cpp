#include "qorderedjsonmodel.h"

#include <iostream>
#include <sstream>
#include <string>

#include "json.hpp"

using json = nlohmann::ordered_json;

class json_validator : public nlohmann::detail::json_sax_dom_parser<json>
{
public:
    json_validator(json& j) : nlohmann::detail::json_sax_dom_parser<json>(j, false) {}

    bool parse_error(std::size_t position,const std::string& last_token, const json::exception& ex)
    {
        std::cerr << "parse error at input byte " << position << "\n"
                  << ex.what() << "\n"
                  << "last read: \"" << last_token << "\""
                  << std::endl;
        return false; // must return false
    }
};


QOrderedJsonModel::QOrderedJsonModel(QObject *parent)
    : QAbstractItemModel(parent),
      m_rootItem (new QJsonTreeItem()),
      m_headers( {QString("Keys"), QString("Value") } )
{

}

QOrderedJsonModel::QOrderedJsonModel(const QString json_file, QObject *parent)
    : QAbstractItemModel(parent),
      m_rootItem (new QJsonTreeItem()),
      m_headers( {QString("Keys"), QString("Value") } )
{
    json json_input = json::parse(json_file.toStdString());

    //a root "node" of a json file always is a json object
    parse_json_object("root:", json_input, m_rootItem);
}


QOrderedJsonModel::~QOrderedJsonModel()
{
    delete m_rootItem;
}

QVariant QOrderedJsonModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() )
        return QVariant();

    if ( role == Qt::ToolTipRole ){
        QJsonTreeItem *item = static_cast<QJsonTreeItem*>(index.internalPointer());
        return item->valueTypeName();
    }

    if ( role == Qt::DisplayRole ){
        QJsonTreeItem *item = static_cast<QJsonTreeItem*>(index.internalPointer());
        return item->data(index.column());
    }
    return QVariant();
}

Qt::ItemFlags QOrderedJsonModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QVariant QOrderedJsonModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return m_headers.at(section);

    return QVariant();
}

QModelIndex QOrderedJsonModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    QJsonTreeItem *parentItem;
    parentItem = parent.isValid() ? static_cast<QJsonTreeItem*>(parent.internalPointer())
                                  : m_rootItem ;

    QJsonTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);

    return QModelIndex();
}

QModelIndex QOrderedJsonModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    QJsonTreeItem *childItem = static_cast<QJsonTreeItem*>(index.internalPointer());
    QJsonTreeItem *parentItem = childItem->parentItem();

    if (parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int QOrderedJsonModel::rowCount(const QModelIndex &parent) const
{
    QJsonTreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    parentItem = parent.isValid() ? static_cast<QJsonTreeItem*>(parent.internalPointer())
                                  : m_rootItem ;

    return parentItem->childCount();
}

int QOrderedJsonModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<QJsonTreeItem*>(parent.internalPointer())->columnCount();

    return m_rootItem->columnCount();
}

void  QOrderedJsonModel::load(const QString &jsonInput)
{
    json json = json::parse(jsonInput.toStdString());

    beginResetModel();
    if ( m_rootItem )
    {
        delete m_rootItem;
        m_rootItem = new QJsonTreeItem();
    }
    parse_json_object("object:", json, m_rootItem);
    endResetModel();
}

bool QOrderedJsonModel::loadFile(const QString &filepath)
{
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "ERROR:  File" << file.fileName() << "fail to open in read mode";
        return false;
    }

    QString jsonInput = file.readAll();
    load(jsonInput);
    return true;
}

bool QOrderedJsonModel::IsValidJson(const QString &jsonInput)
{
    json result;
    json_validator sax(result);

    bool parse_result = json::sax_parse(jsonInput.toStdString(), &sax);

    return !parse_result;
}

QVariant QOrderedJsonModel::fromJsonValue(json::iterator &json_value)
{
    //can't be array or object. they must be parsed whit parse_json_* functions
    Q_ASSERT_X( !json_value->is_array() , "QOrderedJsonModel::fromJsonValue", "type is json::array" );
    Q_ASSERT_X( !json_value->is_object(), "QOrderedJsonModel::fromJsonValue", "type is json::object" );

    if( json_value->type() == json::value_t::string ){
        return QVariant( QString::fromStdString( json_value.value() ) );
    }
    else {
        //all other values are store as a string in a QVariant
        std::stringstream ss;
        ss << json_value.value();
        std::string str;
        ss >> str;
        return  QVariant ( QString::fromStdString(str) );
    }

    return QVariant();
}

// map enum values from nlohmann::json::value_t to JsonValueType
JsonValueType QOrderedJsonModel::fromJsonValueType(json::iterator &json)
{
    switch( json->type() ){
    case json::value_t::string:    return JsonValueType::String;
    case json::value_t::null:      return JsonValueType::Null;
    case json::value_t::boolean:   return JsonValueType::Boolean;
    case json::value_t::binary:    return JsonValueType::Binary;
    case json::value_t::array:     return JsonValueType::Array;
    case json::value_t::object:    return JsonValueType::Object;
    case json::value_t::discarded: return JsonValueType::Undefined;
    case json::value_t::number_float:
    case json::value_t::number_integer:
    case json::value_t::number_unsigned: return JsonValueType::Numeric;
    default:  return JsonValueType::Undefined;
    };
}

QJsonTreeItem * QOrderedJsonModel::parse_json_object(QString key, json &json_value, QJsonTreeItem *parent)
{
    Q_ASSERT_X( json_value.is_object() , "QOrderedJsonModel::parse_json_object", "type is not json::object" );
    Q_ASSERT_X( parent!= nullptr , "QOrderedJsonModel::parse_json_object", "parent item is null" );

    QString stringKey = QString("%1  {%2}").arg(key).arg(json_value.size());
    QJsonTreeItem * objectParent = new QJsonTreeItem(stringKey, QString(), JsonValueType::Object, parent);
    parent->appendChild(objectParent);

    for (auto it = json_value.begin(); it != json_value.end(); ++it)
    {
        if ( it->is_object() ){
            parse_json_object(QString::fromStdString(it.key()), *it, objectParent);
        }
        else if ( it->is_array() ){
            parse_json_array(QString::fromStdString(it.key()), *it, objectParent);
        }
        else{
            QJsonTreeItem * item = new QJsonTreeItem(QString::fromStdString(it.key()),
                                                     fromJsonValue(it),
                                                     fromJsonValueType(it),
                                                     objectParent);
            objectParent->appendChild(item);
        }
    }
    return objectParent;
}

QJsonTreeItem *QOrderedJsonModel::parse_json_array(QString key, json &json_value, QJsonTreeItem *parent)
{
    Q_ASSERT_X( json_value.is_array() , "QOrderedJsonModel::parse_json_object", "type is not json::array" );
    Q_ASSERT_X( parent!= nullptr , "QOrderedJsonModel::parse_json_object", "parent item is null" );

    QString stringKey = QString("%1  [%2]").arg(key).arg(json_value.size());
    QJsonTreeItem * arrayParent = new QJsonTreeItem( stringKey, QString(), JsonValueType::Array, parent);
    parent->appendChild(arrayParent);

    int index = 0;
    for (auto it = json_value.begin(); it != json_value.end(); ++it)
    {
        if ( it->is_object() ){
            parse_json_object(QString::number(index++) + " : ", *it, arrayParent);
        }
        else if ( it->is_array() ){
            parse_json_array(key, *it, arrayParent);
        }
        else {
            QJsonTreeItem * item = new QJsonTreeItem(QString::number(index++) + " : ",
                                                     fromJsonValue(it),
                                                     fromJsonValueType(it),
                                                     arrayParent);
            arrayParent->appendChild(item);
        }
    }
    return arrayParent;
}

