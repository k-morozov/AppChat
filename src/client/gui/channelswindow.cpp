#include "channelswindow.h"
#include "ui_channelswindow.h"
#include <iostream>

ChannelsWindow::ChannelsWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChannelsWindow)
{
    ui->setupUi(this);

    QPalette pail;
    pail.setColor(this->backgroundRole(), Qt::darkCyan);
    this->setPalette(pail);
//    AppChannelsModel *PhoneBookModel = new AppChannelsModel(this);
    update_list_channels();

    ui->listView->show();
    PhoneBookModel = new AppChannelsModel(this);
    connect(ui->listView, SIGNAL(doubleClicked(QModelIndex)),this,SLOT(OnDoubleClicked(QModelIndex)));
}

void ChannelsWindow::update_list_channels() {
//    std::cout << "update_list_channels()" << std::endl;
    QList<QString> channels_name;
    if (client_ptr) {
        channels_name.clear();
        const auto list = client_ptr->get_channels();
        for(const auto& channel : list) {
            channels_name.append(std::to_string(channel).data());
        }
     }
    // @todo memory leak?
    PhoneBookModel = new AppChannelsModel(this);
    PhoneBookModel->populateData(channels_name);

    ui->listView->setModel(PhoneBookModel);
}

ChannelsWindow::~ChannelsWindow()
{
    delete ui;
}

void ChannelsWindow::OnDoubleClicked(const QModelIndex &index) {
    std::cout << "Double clicked, channel_id = : " << PhoneBookModel->get_channel(index.row()) << std::endl;
    client_ptr->set_current_channel(std::stoi(PhoneBookModel->get_channel(index.row())));
    emit sig_join_room(std::stoi(PhoneBookModel->get_channel(index.row())));
}

void ChannelsWindow::on_push_change_room_id_clicked()
{
    auto new_roomid = ui->room_id->text();
    if (new_roomid.isEmpty()) return;

    ui->room_id->clear();
    emit sig_add_channel(new_roomid.toInt());
}


// ******************************************************************************************************

AppChannelsModel::AppChannelsModel(QObject *parent) : QAbstractListModel(parent)
{
}

void AppChannelsModel::populateData(const QList<QString> & channels)
{
    listChannels.clear();
    listChannels = channels;
    return;
}

int AppChannelsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return listChannels.length();
}

int AppChannelsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant AppChannelsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }
    if (index.column() == 0) {
        return listChannels[index.row()];
    }
    return QVariant();
}

QVariant AppChannelsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section == 0) {
            return QString("My channels");
        }
    }
    return QVariant();
}

