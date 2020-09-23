#ifndef CHANNELSWINDOW_H
#define CHANNELSWINDOW_H

#include <QWidget>
#include <QtWidgets>
#include <QAbstractListModel>
#include "client/client.h"

namespace Ui {
class ChannelsWindow;
}

class AppChannelsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    AppChannelsModel(QObject *parent = 0);

    void populateData(const QList<QString> &contactName);

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    std::string get_channel(int index) {
        return listChannels.at(index).toStdString();
    }
private:
    QList<QString> listChannels;


};

class ChannelsWindow : public QWidget
{
    Q_OBJECT

public:
    ChannelsWindow(QWidget *parent = nullptr);
    ~ChannelsWindow();

    void set_client(std::shared_ptr<Client> a_client) {
        client_ptr = a_client;
    }

    void update_list_channels();

signals:
    void sig_add_channel(int);
    void sig_join_room(int);
public slots:
    void OnDoubleClicked(const QModelIndex &index);
private slots:
    void on_push_change_room_id_clicked();

private:
    Ui::ChannelsWindow *ui;
    std::shared_ptr<Client> client_ptr;
    AppChannelsModel *PhoneBookModel;
};

#endif // CHANNELSWINDOW_H
