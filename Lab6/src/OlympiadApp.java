import javax.swing.*;
import javax.swing.border.TitledBorder;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.FileWriter;
import java.io.IOException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.*;
import java.util.List;

public class OlympiadApp extends JFrame {

    private ResourceBundle messages;
    private Locale currentLocale;

    private JLabel titleLabel;
    private JLabel celebrantsLabel;
    private JCheckBox cb1, cb2, cb3;
    private JLabel giftsLabel;
    private JList<String> giftsList;
    private JLabel concertLabel;
    private JRadioButton concertYes, concertNo;
    private JLabel clientLabel;
    private JRadioButton clientYes, clientNo;
    private JLabel totalLabel;
    private JButton saveButton;

    private JLabel dateLongLabel;
    private JLabel dateShortLabel;
    private JComboBox<String> localeBox;

    private final double PRICE_CELEB1 = 1000.0;
    private final double PRICE_CELEB2 = 2000.0;
    private final double PRICE_CELEB3 = 1500.0;
    private final double PRICE_CONCERT = 5000.0;
    private final double PRICE_GIFT1 = 500.0;
    private final double PRICE_GIFT2 = 1500.0;
    private final double PRICE_GIFT3 = 300.0;

    public OlympiadApp() {
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(600, 700);
        setLayout(new BorderLayout(10, 10));

        initLocale();

        JPanel mainPanel = new JPanel();
        mainPanel.setLayout(new BoxLayout(mainPanel, BoxLayout.Y_AXIS));
        mainPanel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

        JPanel topPanel = new JPanel(new FlowLayout(FlowLayout.RIGHT));
        localeBox = new JComboBox<>(new String[]{"ru_RU", "en_US", "en_GB"});
        localeBox.setSelectedItem("ru_RU");
        localeBox.addActionListener(e -> changeLocale());
        topPanel.add(localeBox);
        add(topPanel, BorderLayout.NORTH);

        titleLabel = new JLabel();
        titleLabel.setFont(new Font("Arial", Font.BOLD, 18));
        titleLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
        mainPanel.add(titleLabel);
        mainPanel.add(Box.createRigidArea(new Dimension(0, 10)));

        JPanel celebPanel = new JPanel(new GridLayout(3, 1));
        celebPanel.setBorder(BorderFactory.createTitledBorder(""));
        cb1 = new JCheckBox();
        cb2 = new JCheckBox();
        cb3 = new JCheckBox();
        celebPanel.add(cb1);
        celebPanel.add(cb2);
        celebPanel.add(cb3);
        mainPanel.add(celebPanel);

        JPanel giftPanel = new JPanel(new BorderLayout());
        giftsList = new JList<>(new String[]{"", "", ""});
        giftsList.setSelectionMode(ListSelectionModel.MULTIPLE_INTERVAL_SELECTION);
        JScrollPane scrollPane = new JScrollPane(giftsList);
        scrollPane.setPreferredSize(new Dimension(200, 80));
        giftPanel.add(scrollPane, BorderLayout.CENTER);
        mainPanel.add(giftPanel);

        JPanel concertPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
        concertYes = new JRadioButton();
        concertNo = new JRadioButton();
        concertNo.setSelected(true);
        ButtonGroup concertGroup = new ButtonGroup();
        concertGroup.add(concertYes);
        concertGroup.add(concertNo);
        concertPanel.add(concertYes);
        concertPanel.add(concertNo);
        mainPanel.add(concertPanel);

        JPanel clientPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
        clientYes = new JRadioButton();
        clientNo = new JRadioButton();
        clientNo.setSelected(true);
        ButtonGroup clientGroup = new ButtonGroup();
        clientGroup.add(clientYes);
        clientGroup.add(clientNo);
        clientPanel.add(clientYes);
        clientPanel.add(clientNo);
        mainPanel.add(clientPanel);

        totalLabel = new JLabel();
        totalLabel.setFont(new Font("Arial", Font.BOLD, 16));
        mainPanel.add(totalLabel);

        saveButton = new JButton();
        saveButton.setAlignmentX(Component.CENTER_ALIGNMENT);
        saveButton.addActionListener(e -> saveToFile());
        mainPanel.add(saveButton);

        mainPanel.add(Box.createRigidArea(new Dimension(0, 20)));

        JPanel datePanel = new JPanel(new GridLayout(2, 1));
        dateLongLabel = new JLabel();
        dateShortLabel = new JLabel();
        datePanel.add(dateLongLabel);
        datePanel.add(dateShortLabel);
        mainPanel.add(datePanel);

        add(mainPanel, BorderLayout.CENTER);

        ItemListener listener = e -> calculateTotal();
        cb1.addItemListener(listener);
        cb2.addItemListener(listener);
        cb3.addItemListener(listener);
        giftsList.addListSelectionListener(e -> {
            if (!e.getValueIsAdjusting()) calculateTotal();
        });
        concertYes.addItemListener(listener);
        concertNo.addItemListener(listener);
        clientYes.addItemListener(listener);
        clientNo.addItemListener(listener);

        updateTexts();
        updateDates();
        calculateTotal();

        setLocationRelativeTo(null);
        setVisible(true);
    }

    private void initLocale() {
        currentLocale = new Locale("ru", "RU");
        messages = ResourceBundle.getBundle("MessagesBundle", currentLocale);
    }

    private void changeLocale() {
        String selected = (String) localeBox.getSelectedItem();
        if ("ru_RU".equals(selected)) currentLocale = new Locale("ru", "RU");
        else if ("en_US".equals(selected)) currentLocale = new Locale("en", "US");
        else if ("en_GB".equals(selected)) currentLocale = new Locale("en", "GB");

        messages = ResourceBundle.getBundle("MessagesBundle", currentLocale);
        updateTexts();
        updateDates();
    }

    private void updateTexts() {
        setTitle(messages.getString("title"));
        titleLabel.setText(messages.getString("title"));

        ((TitledBorder) ((JPanel) cb1.getParent()).getBorder()).setTitle(messages.getString("celebTitle"));
        cb1.setText(messages.getString("celeb1") + " (" + PRICE_CELEB1 + ")");
        cb2.setText(messages.getString("celeb2") + " (" + PRICE_CELEB2 + ")");
        cb3.setText(messages.getString("celeb3") + " (" + PRICE_CELEB3 + ")");

        ((TitledBorder) ((JPanel) giftsList.getParent().getParent()).getBorder()).setTitle(messages.getString("giftTitle"));
        DefaultListModel<String> model = new DefaultListModel<>();
        model.addElement(messages.getString("gift1") + " (" + PRICE_GIFT1 + ")");
        model.addElement(messages.getString("gift2") + " (" + PRICE_GIFT2 + ")");
        model.addElement(messages.getString("gift3") + " (" + PRICE_GIFT3 + ")");
        giftsList.setModel(model);

        ((TitledBorder) ((JPanel) concertYes.getParent()).getBorder()).setTitle(messages.getString("concertTitle"));
        concertYes.setText(messages.getString("yes"));
        concertNo.setText(messages.getString("no"));

        ((TitledBorder) ((JPanel) clientYes.getParent()).getBorder()).setTitle(messages.getString("clientTitle"));
        clientYes.setText(messages.getString("yes"));
        clientNo.setText(messages.getString("no"));

        saveButton.setText(messages.getString("saveBtn"));
        calculateTotal();
    }

    private void updateDates() {
        DateFormat dfLong = DateFormat.getDateInstance(DateFormat.FULL, currentLocale);
        DateFormat dfShort = DateFormat.getDateInstance(DateFormat.SHORT, currentLocale);
        Date now = new Date();
        dateLongLabel.setText(messages.getString("dateLong") + ": " + dfLong.format(now));
        dateShortLabel.setText(messages.getString("dateShort") + ": " + dfShort.format(now));
    }

    private void calculateTotal() {
        double total = 0;
        if (cb1.isSelected()) total += PRICE_CELEB1;
        if (cb2.isSelected()) total += PRICE_CELEB2;
        if (cb3.isSelected()) total += PRICE_CELEB3;

        int[] selectedIndices = giftsList.getSelectedIndices();
        for (int idx : selectedIndices) {
            if (idx == 0) total += PRICE_GIFT1;
            if (idx == 1) total += PRICE_GIFT2;
            if (idx == 2) total += PRICE_GIFT3;
        }

        if (concertYes.isSelected()) total += PRICE_CONCERT;

        if (clientYes.isSelected()) total *= 0.9;

        totalLabel.setText(messages.getString("total") + ": " + String.format("%.2f", total));
    }

    private void saveToFile() {
        try (FileWriter writer = new FileWriter("order.txt")) {
            writer.write(messages.getString("title") + "\n");
            writer.write(messages.getString("celebTitle") + ":\n");
            if (cb1.isSelected()) writer.write(" - " + cb1.getText() + "\n");
            if (cb2.isSelected()) writer.write(" - " + cb2.getText() + "\n");
            if (cb3.isSelected()) writer.write(" - " + cb3.getText() + "\n");

            writer.write(messages.getString("giftTitle") + ":\n");
            List<String> selectedGifts = giftsList.getSelectedValuesList();
            for (String gift : selectedGifts) writer.write(" - " + gift + "\n");

            writer.write(messages.getString("concertTitle") + ": " + (concertYes.isSelected() ? messages.getString("yes") : messages.getString("no")) + "\n");
            writer.write(messages.getString("clientTitle") + ": " + (clientYes.isSelected() ? messages.getString("yes") : messages.getString("no")) + "\n");
            writer.write(totalLabel.getText() + "\n");
            writer.write(dateLongLabel.getText() + "\n");
            writer.write(dateShortLabel.getText() + "\n");

            JOptionPane.showMessageDialog(this, messages.getString("saved"));
        } catch (IOException ex) {
            JOptionPane.showMessageDialog(this, "Error: " + ex.getMessage());
        }
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(OlympiadApp::new);
    }
}