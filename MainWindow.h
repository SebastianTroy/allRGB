#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void onLoadImagePreviewRequested();
    void onAllRgbPreviewRequested();
    void onCropAndBeginRequested();

protected:
    virtual void showEvent(QShowEvent* event) override;

private:
    Ui::MainWindow *ui;

    void OpenImagePreviewWindow(const QImage& toPreview);
};
#endif // MAINWINDOW_H
