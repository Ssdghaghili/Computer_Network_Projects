#include <QPainter>
#include <QImage>
#include <QFile>
#include <QVector>
#include <QFont>

/**
 * Plots and saves a diagram.
 * Note that this function will sample data if length of array is more than image width.
 *
 * @param dataArray Contains values to be plotted.
 * @param fileName Output file full location, including file path, name and format(png).
 */

void plotDiagram(const QVector<int>& dataArray, const QString& fileName) {
    int width = 800;
    int height = 600;

    QImage image(width, height, QImage::Format_ARGB32);
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(image.rect(), Qt::white);

    int margin = 70;
    int plotWidth = width - 2 * margin;
    int plotHeight = height - 2 * margin;

    int minValue = *std::min_element(dataArray.begin(), dataArray.end());
    int maxValue = *std::max_element(dataArray.begin(), dataArray.end());

    int n = dataArray.size();
    int maxPoints = plotWidth;
    QVector<int> scaledData;

    int step = std::ceil((double)n / maxPoints);
    step=step>0?step:1;
    if (n > maxPoints) {
        for (int i = 0; i < n; i += step)
            scaledData.append(dataArray[i]);
        n = scaledData.size();
    } else
        scaledData = dataArray;

    for (int& value : scaledData)
        value = height - margin - ((value - minValue) * plotHeight) / (maxValue - minValue);

    painter.setPen(QPen(Qt::black, 2));
    painter.drawLine(margin, height - margin, width - margin, height - margin);
    painter.drawLine(margin, margin, margin, height - margin);

    int numYTicks = 10;
    QFont font = painter.font();
    font.setPointSize(8);
    painter.setFont(font);
    for (int i = 0; i <= numYTicks; ++i) {
        int y = height - margin - (i * plotHeight / numYTicks);
        int value = minValue + (i * (maxValue - minValue) / numYTicks);

        painter.drawLine(margin - 5, y, margin, y);
        painter.drawText(margin - 50, y + 5, QString::number(value));
    }

    int numXTicks = 10;
    for (int i = 0; i <= numXTicks; ++i) {
        int x = margin + (i * plotWidth / numXTicks);
        int index = i * (n - 1) / numXTicks;

        painter.drawLine(x, height - margin, x, height - margin + 5);
        painter.drawText(x - 10, height - margin + 20, QString::number(index*step));
    }

    painter.setPen(QPen(Qt::blue, 2));
    for (int i = 0; i < n - 1; ++i) {
        int x1 = margin + i * (plotWidth / (n - 1));
        int y1 = scaledData[i];
        int x2 = margin + (i + 1) * (plotWidth / (n - 1));
        int y2 = scaledData[i + 1];

        painter.drawLine(x1, y1, x2, y2);
    }

    font.setPointSize(10);
    painter.setFont(font);
    painter.setPen(Qt::black);
    painter.drawText(width / 2 - 40, height - 10, "Tick");
    painter.save();
    painter.translate(10, height / 2 + 40);
    painter.rotate(-90);
    painter.drawText(0, 0, "Packets in Queue");
    painter.restore();

    font.setPointSize(12);
    font.setBold(true);
    painter.setFont(font);
    painter.drawText(width / 2 - 80, margin / 2, "Queue on Router 0");

    painter.end();

    if (!image.save(fileName))
        qWarning("Couldn't save the image.");
}
