# # import sys
# # import os
# # import re
# # import time
# # from datetime import datetime
# # from PyQt5.QtWidgets import (
# #     QApplication, QMainWindow, QTableView, QTextEdit, QVBoxLayout, QHBoxLayout,
# #     QWidget, QPushButton, QLabel, QMessageBox, QInputDialog, QHeaderView,
# #     QSplitter, QStatusBar, QLineEdit, QComboBox, QDialog, QDialogButtonBox,
# #     QMenuBar, QMenu, QAction, QToolBar, QToolButton, QTabWidget, QProgressBar,
# #     QFileDialog, QDateEdit, QTreeWidget, QTreeWidgetItem
# # )
# # from PyQt5.QtCore import Qt, QProcess, QTimer, QPropertyAnimation, QEasingCurve, QAbstractTableModel, QSize, QDate
# # from PyQt5.QtGui import QFont, QIcon, QPalette, QColor, QTextCharFormat, QSyntaxHighlighter
# # from watchdog.observers import Observer
# # from watchdog.events import FileSystemEventHandler

# # # Packet Highlighter for Syntax Highlighting in Details View
# # class PacketHighlighter(QSyntaxHighlighter):
# #     def __init__(self, parent=None):
# #         super().__init__(parent)
# #         self.highlighting_rules = []
# #         # IP Address
# #         ip_format = QTextCharFormat()
# #         ip_format.setForeground(QColor("#1E90FF"))
# #         ip_format.setFontWeight(QFont.Bold)
# #         self.highlighting_rules.append((r'\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\b', ip_format))
# #         # Protocols
# #         protocol_format = QTextCharFormat()
# #         protocol_format.setForeground(QColor("#32CD32"))
# #         protocol_format.setFontWeight(QFont.Bold)
# #         self.highlighting_rules.append((r'\b(TCP|UDP|ICMP|ARP|IPv6|HTTP|TLS|SMTP|DNS)\b', protocol_format))
# #         # Ports
# #         port_format = QTextCharFormat()
# #         port_format.setForeground(QColor("#FF4500"))
# #         self.highlighting_rules.append((r'\bPort: \d+\b', port_format))
# #         # MAC Address
# #         mac_format = QTextCharFormat()
# #         mac_format.setForeground(QColor("#9932CC"))
# #         self.highlighting_rules.append((r'\b[0-9A-Fa-f]{2}(:[0-9A-Fa-f]{2}){5}\b', mac_format))
# #         # Timestamp
# #         time_format = QTextCharFormat()
# #         time_format.setForeground(QColor("#FF69B4"))
# #         self.highlighting_rules.append((r'\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d+', time_format))
# #         # Application Data
# #         app_data_format = QTextCharFormat()
# #         app_data_format.setForeground(QColor("#FFD700"))
# #         self.highlighting_rules.append((r'Application Data: .+', app_data_format))
# #         # Payload Hex + ASCII
# #         payload_format = QTextCharFormat()
# #         payload_format.setForeground(QColor("#00CED1"))
# #         self.highlighting_rules.append((r'Payload \(Hex \+ ASCII\):[\s\S]+?(?=\n\n|\Z)', payload_format))

# #     def highlightBlock(self, text: str) -> None:
# #         for pattern, format in self.highlighting_rules:
# #             for match in re.finditer(pattern, text, re.MULTILINE):
# #                 start, end = match.start(), match.end()
# #                 self.setFormat(start, end - start, format)

# # # Packet Class with 7 Layers and Payload
# # class Packet:
# #     def __init__(self, number: str, timestamp: str, src_ip: str, dst_ip: str, protocol: str, size: str, details: dict):
# #         self.number = number
# #         self.timestamp = timestamp
# #         self.src_ip = src_ip
# #         self.dst_ip = dst_ip
# #         self.protocol = protocol
# #         self.size = size
# #         self.details = details  # Dictionary for sections, layers, and payload

# # # File Log Class
# # class FileLog:
# #     def __init__(self, timestamp: str, operation: str, file_path: str, details: str, color: QColor = QColor(0, 0, 0)):
# #         self.timestamp = timestamp
# #         self.operation = operation
# #         self.file_path = file_path
# #         self.details = details
# #         self.color = color

# # # Packet Table Model
# # class PacketTableModel(QAbstractTableModel):
# #     def __init__(self, packets: list, parent=None):
# #         super().__init__(parent)
# #         self.packets = packets

# #     def rowCount(self, index) -> int:
# #         return len(self.packets)

# #     def columnCount(self, index) -> int:
# #         return 6

# #     def data(self, index, role: int):
# #         if not index.isValid():
# #             return None
# #         if role == Qt.DisplayRole:
# #             packet = self.packets[index.row()]
# #             return [packet.number, packet.timestamp, packet.src_ip, packet.dst_ip, packet.protocol or "Unknown", packet.size or "0"][index.column()]
# #         if role == Qt.BackgroundRole:
# #             packet = self.packets[index.row()]
# #             color = QColor(255, 255, 255)
# #             if packet.protocol == "TCP":
# #                 color = QColor(220, 230, 255)
# #             elif packet.protocol == "UDP":
# #                 color = QColor(220, 255, 220)
# #             elif packet.protocol == "ICMP":
# #                 color = QColor(255, 220, 220)
# #             elif packet.protocol == "ARP":
# #                 color = QColor(240, 220, 255)
# #             elif packet.protocol == "IPv6":
# #                 color = QColor(255, 230, 210)
# #             elif packet.protocol == "112":
# #                 color = QColor(230, 210, 255)
# #             elif packet.protocol == "None":
# #                 color = QColor(245, 245, 220)
# #             return color
# #         return None

# #     def headerData(self, section: int, orientation: Qt.Orientation, role: int):
# #         if role != Qt.DisplayRole:
# #             return None
# #         headers = ["Packet #", "Timestamp", "Source IP", "Destination IP", "Protocol", "Size"]
# #         return headers[section] if orientation == Qt.Horizontal else str(section + 1)

# # # File Log Table Model
# # class FileLogTableModel(QAbstractTableModel):
# #     def __init__(self, logs: list, parent=None):
# #         super().__init__(parent)
# #         self.logs = logs

# #     def rowCount(self, index) -> int:
# #         return len(self.logs)

# #     def columnCount(self, index) -> int:
# #         return 4

# #     def data(self, index, role: int):
# #         if not index.isValid():
# #             return None
# #         if role == Qt.DisplayRole:
# #             log = self.logs[index.row()]
# #             return [log.timestamp, log.operation, log.file_path, log.details][index.column()]
# #         if role == Qt.BackgroundRole:
# #             log = self.logs[index.row()]
# #             color = QColor(255, 255, 255)
# #             if log.operation == "CREATE":
# #                 color = QColor(220, 255, 220)
# #             elif log.operation == "UPDATE":
# #                 color = QColor(255, 255, 200)
# #             elif log.operation == "DELETE":
# #                 color = QColor(255, 220, 220)
# #             elif log.operation == "MOVE":
# #                 color = QColor(220, 220, 255)
# #             return color
# #         if role == Qt.ForegroundRole:
# #             log = self.logs[index.row()]
# #             return log.color
# #         return None

# #     def headerData(self, section: int, orientation: Qt.Orientation, role: int):
# #         if role != Qt.DisplayRole:
# #             return None
# #         headers = ["Timestamp", "Operation", "File Path", "Details"]
# #         return headers[section] if orientation == Qt.Horizontal else str(section + 1)

# # # File Monitor for Watching Shared Folder
# # class FileMonitor(FileSystemEventHandler):
# #     def __init__(self, folder_path: str, parent):
# #         super().__init__()
# #         self.folder_path = folder_path
# #         self.parent = parent

# #     def on_created(self, event) -> None:
# #         if not event.is_directory:
# #             self.log_event("CREATE", event.src_path, f"File created: {event.src_path}")

# #     def on_modified(self, event) -> None:
# #         if not event.is_directory:
# #             self.log_event("UPDATE", event.src_path, f"File modified: {event.src_path}")

# #     def on_deleted(self, event) -> None:
# #         if not event.is_directory:
# #             self.log_event("DELETE", event.src_path, f"File deleted: {event.src_path}")

# #     def on_moved(self, event) -> None:
# #         if not event.is_directory:
# #             self.log_event("MOVE", event.dest_path, f"File moved from {event.src_path} to {event.dest_path}")

# #     def log_event(self, operation: str, file_path: str, details: str) -> None:
# #         timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
# #         color = QColor(0, 0, 0)
# #         log = FileLog(timestamp, operation, file_path, details, color)
# #         log_dir = r"C:\Logs"
# #         if not os.path.exists(log_dir):
# #             os.makedirs(log_dir)
# #         with open(os.path.join(log_dir, "file_operations.log"), "a", encoding="utf-8") as f:
# #             f.write(f"[{timestamp}] {operation}: {details}\n")
# #         self.parent.file_logs.append(log)
# #         self.parent.file_log_model.layoutChanged.emit()

# # # Interface Selection Dialog
# # class InterfaceDialog(QDialog):
# #     def __init__(self, interfaces: list, parent=None):
# #         super().__init__(parent)
# #         self.setWindowTitle("Select Network Interface")
# #         self.setModal(True)
# #         layout = QVBoxLayout(self)
# #         self.combo = QComboBox()
# #         for index, desc in interfaces:
# #             self.combo.addItem(f"{index}: {desc}", index)
# #         layout.addWidget(self.combo)
# #         buttons = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
# #         buttons.accepted.connect(self.accept)
# #         buttons.rejected.connect(self.reject)
# #         layout.addWidget(buttons)

# #     def selected_index(self) -> int:
# #         return self.combo.currentData()

# # # Date Selection Dialog
# # class DateSelectionDialog(QDialog):
# #     def __init__(self, parent=None):
# #         super().__init__(parent)
# #         self.setWindowTitle("Select Date")
# #         self.setModal(True)
# #         layout = QVBoxLayout(self)
# #         self.date_edit = QDateEdit()
# #         self.date_edit.setCalendarPopup(True)
# #         self.date_edit.setDate(QDate.currentDate())
# #         self.date_edit.setDisplayFormat("yyyy-MM-dd")
# #         layout.addWidget(self.date_edit)
# #         buttons = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
# #         buttons.accepted.connect(self.accept)
# #         buttons.rejected.connect(self.reject)
# #         layout.addWidget(buttons)

# #     def selected_date(self) -> str:
# #         return self.date_edit.date().toString("yyyy-MM-dd")

# # # Main Network Monitor Window
# # class NetworkMonitor(QMainWindow):
# #     def __init__(self):
# #         super().__init__()
# #         self.setWindowTitle("Advanced Network Monitor")
# #         self.setGeometry(100, 100, 1400, 900)
# #         self.packets: list[Packet] = []
# #         self.file_logs: list[FileLog] = []
# #         self.process = None
# #         self.current_mode = None
# #         self.is_paused = False
# #         self.observer = None
# #         self.init_ui()
# #         self.apply_styles()
# #         self.update_timer = QTimer(self)
# #         self.update_timer.timeout.connect(self.update_tables)
# #         self.update_timer.start(300)
# #         self.search_timer = QTimer(self)
# #         self.search_timer.setSingleShot(True)
# #         self.search_timer.timeout.connect(self.perform_search)
# #         self.reload_timer = QTimer(self)
# #         self.reload_timer.timeout.connect(self.reload_data)
# #         self.reload_timer.start(1000)
# #         self.layer_details_visible = False

# #     def init_ui(self) -> None:
# #         # Menu Bar
# #         menubar = QMenuBar(self)
# #         self.setMenuBar(menubar)
# #         file_menu = menubar.addMenu("File")
# #         tools_menu = menubar.addMenu("Tools")
# #         view_menu = menubar.addMenu("View")
# #         help_menu = menubar.addMenu("Help")

# #         save_action = QAction(QIcon.fromTheme("document-save"), "Save Capture", self)
# #         save_action.triggered.connect(self.save_capture)
# #         file_menu.addAction(save_action)

# #         export_action = QAction(QIcon.fromTheme("document-export"), "Export to PCAP", self)
# #         export_action.triggered.connect(lambda: QMessageBox.information(self, "Info", "Export to PCAP coming soon!"))
# #         file_menu.addAction(export_action)

# #         preferences_action = QAction(QIcon.fromTheme("preferences-system"), "Preferences", self)
# #         preferences_action.triggered.connect(lambda: QMessageBox.information(self, "Info", "Preferences coming soon!"))
# #         tools_menu.addAction(preferences_action)

# #         toggle_layers_action = QAction(QIcon.fromTheme("view-list-tree"), "Toggle Layer Details", self, checkable=True)
# #         toggle_layers_action.toggled.connect(self.toggle_layer_details)
# #         view_menu.addAction(toggle_layers_action)

# #         about_action = QAction(QIcon.fromTheme("help-about"), "About", self)
# #         about_action.triggered.connect(self.show_about)
# #         help_menu.addAction(about_action)

# #         # Toolbar
# #         toolbar = QToolBar("Main Toolbar", self)
# #         toolbar.setIconSize(QSize(32, 32))
# #         toolbar.setToolButtonStyle(Qt.ToolButtonTextUnderIcon)
# #         self.addToolBar(Qt.TopToolBarArea, toolbar)
# #         self.search_input = QLineEdit()
# #         self.search_input.setPlaceholderText("Search packets/logs...")
# #         self.search_input.textChanged.connect(lambda: self.search_timer.start(300))
# #         toolbar.addWidget(self.search_input)
# #         search_btn = QToolButton()
# #         search_btn.setIcon(QIcon.fromTheme("system-search"))
# #         search_btn.setToolTip("Search")
# #         search_btn.clicked.connect(self.perform_search)
# #         toolbar.addWidget(search_btn)

# #         # Main Layout
# #         main_widget = QWidget()
# #         self.setCentralWidget(main_widget)
# #         main_layout = QVBoxLayout(main_widget)
# #         main_layout.setContentsMargins(20, 20, 20, 20)
# #         main_layout.setSpacing(15)

# #         # Header
# #         header = QLabel("Advanced Network Monitor")
# #         header.setFont(QFont("Segoe UI", 28, QFont.Bold))
# #         header.setAlignment(Qt.AlignCenter)
# #         main_layout.addWidget(header)

# #         # Tabs
# #         self.tab_widget = QTabWidget()
# #         main_layout.addWidget(self.tab_widget)

# #         # Packet Tab
# #         packet_tab = QWidget()
# #         packet_tab_layout = QVBoxLayout(packet_tab)

# #         # Control Panel
# #         control_panel = QWidget()
# #         control_layout = QHBoxLayout(control_panel)
# #         control_layout.setAlignment(Qt.AlignCenter)
# #         control_layout.setSpacing(20)

# #         self.stored_logs_btn = QPushButton(QIcon.fromTheme("document-open"), "Stored Logs")
# #         self.stored_logs_btn.setFixedSize(180, 50)
# #         self.stored_logs_btn.clicked.connect(self.show_stored_logs)
# #         self.stored_logs_btn.setToolTip("View stored packet logs")
# #         control_layout.addWidget(self.stored_logs_btn)

# #         self.current_traffic_btn = QPushButton(QIcon.fromTheme("network-connect"), "Current Traffic")
# #         self.current_traffic_btn.setFixedSize(180, 50)
# #         self.current_traffic_btn.clicked.connect(self.show_current_traffic)
# #         self.current_traffic_btn.setToolTip("Capture live traffic")
# #         control_layout.addWidget(self.current_traffic_btn)

# #         self.delete_logs_btn = QPushButton(QIcon.fromTheme("edit-delete"), "Delete Logs")
# #         self.delete_logs_btn.setFixedSize(180, 50)
# #         self.delete_logs_btn.clicked.connect(self.delete_stored_traffic)
# #         self.delete_logs_btn.setToolTip("Delete stored traffic")
# #         control_layout.addWidget(self.delete_logs_btn)

# #         self.stop_capture_btn = QPushButton(QIcon.fromTheme("media-playback-stop"), "Stop Capture")
# #         self.stop_capture_btn.setFixedSize(180, 50)
# #         self.stop_capture_btn.clicked.connect(self.stop_capture)
# #         self.stop_capture_btn.setVisible(False)
# #         self.stop_capture_btn.setToolTip("Stop capturing traffic")
# #         control_layout.addWidget(self.stop_capture_btn)

# #         self.pause_resume_btn = QPushButton(QIcon.fromTheme("media-playback-pause"), "Pause/Resume")
# #         self.pause_resume_btn.setFixedSize(180, 50)
# #         self.pause_resume_btn.clicked.connect(self.toggle_pause_resume)
# #         self.pause_resume_btn.setVisible(False)
# #         self.pause_resume_btn.setToolTip("Pause/resume capture")
# #         control_layout.addWidget(self.pause_resume_btn)

# #         self.clear_table_btn = QPushButton(QIcon.fromTheme("edit-clear"), "Clear Table")
# #         self.clear_table_btn.setFixedSize(180, 50)
# #         self.clear_table_btn.clicked.connect(self.clear_table)
# #         self.clear_table_btn.setToolTip("Clear the table")
# #         control_layout.addWidget(self.clear_table_btn)

# #         packet_tab_layout.addWidget(control_panel)

# #         # Filter Panel
# #         filter_panel = QWidget()
# #         filter_layout = QHBoxLayout(filter_panel)
# #         filter_label = QLabel("Filter by Date:")
# #         filter_label.setFont(QFont("Segoe UI", 12))
# #         self.date_filter_btn = QPushButton("Select Date")
# #         self.date_filter_btn.setFixedSize(100, 30)
# #         self.date_filter_btn.clicked.connect(self.show_date_dialog)
# #         filter_layout.addWidget(filter_label)
# #         filter_layout.addWidget(self.date_filter_btn)
# #         packet_tab_layout.addWidget(filter_panel)

# #         # Splitter for Packets and Details
# #         splitter = QSplitter(Qt.Vertical)
# #         self.packet_view = QTableView()
# #         self.packet_model = PacketTableModel(self.packets, self)
# #         self.packet_view.setModel(self.packet_model)
# #         self.packet_view.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
# #         self.packet_view.setSelectionMode(QTableView.SingleSelection)
# #         self.packet_view.setSortingEnabled(True)
# #         self.packet_view.clicked.connect(self.show_packet_details)
# #         splitter.addWidget(self.packet_view)

# #         self.details_widget = QWidget()
# #         details_layout = QVBoxLayout(self.details_widget)
# #         self.layer_tree = QTreeWidget()
# #         self.layer_tree.setHeaderHidden(True)
# #         self.layer_tree.setVisible(False)
# #         details_layout.addWidget(self.layer_tree)
# #         self.details_text = QTextEdit()
# #         self.details_text.setReadOnly(True)
# #         self.details_text.setFont(QFont("Consolas", 12))
# #         self.highlighter = PacketHighlighter(self.details_text.document())
# #         details_layout.addWidget(self.details_text)
# #         splitter.addWidget(self.details_widget)
# #         splitter.setSizes([600, 300])
# #         packet_tab_layout.addWidget(splitter)

# #         # File Log Tab
# #         file_log_tab = QWidget()
# #         file_log_tab_layout = QVBoxLayout(file_log_tab)
# #         self.file_log_view = QTableView()
# #         self.file_log_model = FileLogTableModel(self.file_logs, self)
# #         self.file_log_view.setModel(self.file_log_model)
# #         self.file_log_view.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
# #         self.file_log_view.setSelectionMode(QTableView.SingleSelection)
# #         self.file_log_view.setSortingEnabled(True)
# #         self.file_log_view.clicked.connect(self.show_file_log_details)
# #         file_log_tab_layout.addWidget(self.file_log_view)

# #         self.tab_widget.addTab(packet_tab, QIcon.fromTheme("network-wired"), "Packets")
# #         self.tab_widget.addTab(file_log_tab, QIcon.fromTheme("folder"), "File Logs")

# #         # Status Bar
# #         self.status_bar = QStatusBar()
# #         self.setStatusBar(self.status_bar)
# #         self.progress_bar = QProgressBar()
# #         self.progress_bar.setVisible(False)
# #         self.status_bar.addPermanentWidget(self.progress_bar)

# #     def apply_styles(self) -> None:
# #         palette = QPalette()
# #         palette.setColor(QPalette.Window, QColor(240, 240, 245))
# #         palette.setColor(QPalette.WindowText, QColor(30, 30, 30))
# #         palette.setColor(QPalette.Base, QColor(255, 255, 255))
# #         palette.setColor(QPalette.AlternateBase, QColor(245, 245, 250))
# #         palette.setColor(QPalette.Text, QColor(30, 30, 30))
# #         palette.setColor(QPalette.Button, QColor(220, 220, 220))
# #         palette.setColor(QPalette.ButtonText, QColor(30, 30, 30))
# #         palette.setColor(QPalette.Highlight, QColor(0, 122, 255))
# #         palette.setColor(QPalette.HighlightedText, QColor(255, 255, 255))
# #         QApplication.setPalette(palette)

# #         self.setStyleSheet("""
# #             QMainWindow {
# #                 background-color: #F0F0F5;
# #             }
# #             QLabel {
# #                 color: #1E1E1E;
# #                 font-family: 'Segoe UI';
# #             }
# #             QPushButton {
# #                 background-color: #E6E6E6;
# #                 border: 1px solid #B0B0B0;
# #                 border-radius: 8px;
# #                 padding: 10px;
# #                 font-family: 'Segoe UI';
# #                 font-size: 14px;
# #                 font-weight: 600;
# #                 color: #1E1E1E;
# #             }
# #             QPushButton:hover {
# #                 background-color: #D6D6D6;
# #                 border: 1px solid #A0A0A0;
# #             }
# #             QPushButton:pressed {
# #                 background-color: #C6C6C6;
# #             }
# #             QTableView {
# #                 background-color: #FFFFFF;
# #                 color: #1E1E1E;
# #                 gridline-color: #E0E0E0;
# #                 selection-background-color: #007AFF;
# #                 selection-color: #FFFFFF;
# #                 font-family: 'Segoe UI';
# #                 font-size: 13px;
# #                 border: 1px solid #D0D0D0;
# #                 border-radius: 5px;
# #             }
# #             QHeaderView::section {
# #                 background-color: #F5F5F5;
# #                 color: #1E1E1E;
# #                 padding: 8px;
# #                 border: 1px solid #D0D0D0;
# #                 font-family: 'Segoe UI';
# #                 font-size: 13px;
# #                 font-weight: 600;
# #             }
# #             QTextEdit {
# #                 background-color: #FFFFFF;
# #                 color: #1E1E1E;
# #                 border: 1px solid #D0D0D0;
# #                 border-radius: 5px;
# #                 font-family: 'Consolas';
# #                 font-size: 13px;
# #             }
# #             QLineEdit {
# #                 background-color: #FFFFFF;
# #                 color: #1E1E1E;
# #                 border: 1px solid #D0D0D0;
# #                 border-radius: 5px;
# #                 padding: 6px;
# #                 font-family: 'Segoe UI';
# #                 font-size: 13px;
# #             }
# #             QLineEdit:focus {
# #                 border: 2px solid #007AFF;
# #             }
# #             QSplitter::handle {
# #                 background-color: #D0D0D0;
# #                 width: 5px;
# #             }
# #             QStatusBar {
# #                 background-color: #F5F5F5;
# #                 color: #1E1E1E;
# #                 font-family: 'Segoe UI';
# #                 font-size: 13px;
# #             }
# #             QTabWidget::pane {
# #                 border: 1px solid #D0D0D0;
# #                 border-radius: 5px;
# #             }
# #             QTabBar::tab {
# #                 background-color: #E6E6E6;
# #                 color: #1E1E1E;
# #                 padding: 12px;
# #                 border-top-left-radius: 5px;
# #                 border-top-right-radius: 5px;
# #                 font-family: 'Segoe UI';
# #                 font-size: 14px;
# #             }
# #             QTabBar::tab:selected {
# #                 background-color: #FFFFFF;
# #                 border-bottom: 2px solid #007AFF;
# #             }
# #             QProgressBar {
# #                 background-color: #E6E6E6;
# #                 border: 1px solid #D0D0D0;
# #                 text-align: center;
# #                 color: #1E1E1E;
# #                 border-radius: 5px;
# #             }
# #             QProgressBar::chunk {
# #                 background-color: #007AFF;
# #                 border-radius: 5px;
# #             }
# #             QToolBar {
# #                 background-color: #F5F5F5;
# #                 border-bottom: 1px solid #D0D0D0;
# #             }
# #             QTreeWidget {
# #                 background-color: #FFFFFF;
# #                 color: #1E1E1E;
# #                 border: 1px solid #D0D0D0;
# #                 border-radius: 5px;
# #                 font-family: 'Segoe UI';
# #                 font-size: 13px;
# #             }
# #             QTreeWidget::item {
# #                 padding: 5px;
# #             }
# #             QTreeWidget::item:hover {
# #                 background-color: #E6F0FA;
# #             }
# #         """)

# #     def animate_button(self, button: QPushButton) -> None:
# #         animation = QPropertyAnimation(button, b"geometry")
# #         animation.setDuration(200)
# #         animation.setEasingCurve(QEasingCurve.OutBounce)
# #         rect = button.geometry()
# #         animation.setStartValue(rect)
# #         animation.setKeyValueAt(0.5, rect.adjusted(5, 5, -5, -5))
# #         animation.setEndValue(rect)
# #         animation.start()

# #     def update_tables(self) -> None:
# #         self.packet_model.layoutChanged.emit()
# #         self.file_log_model.layoutChanged.emit()
# #         packet_scrollbar = self.packet_view.verticalScrollBar()
# #         file_log_scrollbar = self.file_log_view.verticalScrollBar()
# #         if packet_scrollbar.maximum() == 0 or packet_scrollbar.value() >= packet_scrollbar.maximum() - 10:
# #             self.packet_view.scrollToBottom()
# #         if file_log_scrollbar.maximum() == 0 or file_log_scrollbar.value() >= file_log_scrollbar.maximum() - 10:
# #             self.file_log_view.scrollToBottom()

# #     def reload_data(self) -> None:
# #         if self.current_mode == "stored":
# #             self.load_stored_logs()
# #         elif self.current_mode == "current" and self.process and not self.is_paused:
# #             self.read_sniffer_output()
# #         self.update_tables()

# #     def show_stored_logs(self) -> None:
# #         self.current_mode = "stored"
# #         self.packets.clear()
# #         self.details_text.clear()
# #         self.layer_tree.clear()
# #         self.stop_capture_btn.setVisible(False)
# #         self.pause_resume_btn.setVisible(False)
# #         self.progress_bar.setVisible(True)
# #         self.progress_bar.setRange(0, 0)
# #         self.status_bar.showMessage("Loading stored logs...")
# #         self.load_stored_logs()
# #         self.progress_bar.setVisible(False)
# #         self.status_bar.showMessage(f"Stored logs loaded ({len(self.packets)} packets)")
# #         self.animate_button(self.stored_logs_btn)

# #     def load_stored_logs(self) -> None:
# #         log_file = r"C:\Logs\file_operations.log"
# #         self.packets.clear()
# #         try:
# #             if not os.path.exists(log_file):
# #                 QMessageBox.information(self, "Info", "No stored logs found.")
# #                 return

# #             with open(log_file, "r", encoding="utf-8") as f:
# #                 content = f.read()

# #             # Split content by packet blocks, but only include blocks starting with "===== Packet #"
# #             packet_blocks = re.split(r'===== Packet #(\d+) =====', content)[1:]
# #             packet_blocks = [(packet_blocks[i], packet_blocks[i + 1]) for i in range(0, len(packet_blocks) - 1, 2)]

# #             for packet_number, block in packet_blocks:
# #                 # Skip any non-packet content (e.g., "[2025-04-28 ...]") within the block
# #                 lines = block.strip().split("\n")
# #                 if not lines:
# #                     continue

# #                 timestamp = ""
# #                 src_ip = "Unknown"
# #                 dst_ip = "Unknown"
# #                 protocol = "Unknown"
# #                 size = "0"
# #                 details = {}
# #                 current_section = None

# #                 for line in lines:
# #                     line = line.strip()
# #                     if not line:
# #                         continue

# #                     # Skip lines that match the CRUD log format (e.g., "[2025-04-28 ...]")
# #                     if re.match(r'\[\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\]', line):
# #                         continue

# #                     # Detect section headers
# #                     section_match = re.match(r'\[(.*?)\]', line)
# #                     if section_match:
# #                         current_section = section_match.group(1)
# #                         details[current_section] = []
# #                         continue

# #                     # Parse specific fields
# #                     if current_section == "Timing Information":
# #                         if line.startswith("Timestamp: "):
# #                             timestamp = line.split("Timestamp: ", 1)[1].strip()
# #                         elif line.startswith("Captured Length: "):
# #                             size = line.split("Captured Length: ", 1)[1].strip().split()[0]
# #                     elif current_section == "Layer 3 - Network":
# #                         if line.startswith("Source IP: "):
# #                             src_ip = line.split("Source IP: ", 1)[1].strip()
# #                         elif line.startswith("Destination IP: "):
# #                             dst_ip = line.split("Destination IP: ", 1)[1].strip()
# #                         elif line.startswith("Protocol: "):
# #                             protocol = line.split("Protocol: ", 1)[1].strip()
# #                     elif current_section:
# #                         details[current_section].append(line)

# #                 # Convert lists to strings for each section
# #                 for section in details:
# #                     details[section] = "\n".join([l for l in details[section] if l.strip()])

# #                 if timestamp:
# #                     packet = Packet(packet_number, timestamp, src_ip, dst_ip, protocol, size, details)
# #                     self.packets.append(packet)

# #             self.packet_model.layoutChanged.emit()
# #             if not self.packets:
# #                 QMessageBox.information(self, "Info", "No valid packets found in the log file.")

# #         except Exception as e:
# #             QMessageBox.warning(self, "Error", f"Failed to load logs: {str(e)}")
# #             with open(r"C:\Logs\debug.log", "a", encoding="utf-8") as f:
# #                 f.write(f"[{time.strftime('%Y-%m-%d %H:%M:%S')}] Error loading logs: {str(e)}\n")

# #     def apply_date_filter(self, date_str: str) -> None:
# #         if not date_str:
# #             return
# #         try:
# #             datetime.strptime(date_str, "%Y-%m-%d")
# #             filtered_packets = [p for p in self.packets if p.timestamp.startswith(date_str)]
# #             self.packets = filtered_packets
# #             self.update_tables()
# #             self.status_bar.showMessage(f"Filtered packets for {date_str} ({len(self.packets)} packets)")
# #         except ValueError:
# #             QMessageBox.warning(self, "Warning", "Invalid date format")

# #     def show_current_traffic(self) -> None:
# #         self.current_mode = "current"
# #         self.packets.clear()
# #         self.details_text.clear()
# #         self.layer_tree.clear()
# #         self.stop_capture_btn.setVisible(True)
# #         self.pause_resume_btn.setVisible(True)
# #         self.is_paused = False
# #         self.pause_resume_btn.setText("Pause")
# #         self.pause_resume_btn.setIcon(QIcon.fromTheme("media-playback-pause"))
# #         self.progress_bar.setVisible(True)
# #         self.progress_bar.setRange(0, 0)
# #         self.status_bar.showMessage("Fetching network interfaces...")

# #         interfaces = self.get_network_interfaces()
# #         if not interfaces:
# #             QMessageBox.critical(self, "Error", "No network interfaces found or failed to run name.exe")
# #             self.stop_capture()
# #             return

# #         dialog = InterfaceDialog(interfaces, self)
# #         if dialog.exec_() != QDialog.Accepted:
# #             self.stop_capture()
# #             return
# #         interface_index = dialog.selected_index()

# #         folder_path = QFileDialog.getExistingDirectory(self, "Select Shared Folder", os.path.expanduser("~"))
# #         if not folder_path:
# #             self.stop_capture()
# #             return

# #         self.start_file_monitoring(folder_path)
# #         self.start_sniffer_process(interface_index, folder_path)
# #         self.animate_button(self.current_traffic_btn)

# #     def get_network_interfaces(self) -> list:
# #         program = r"C:\Users\kmit\Desktop\name\x64\Debug\name.exe"
# #         if not os.path.exists(program):
# #             return []
# #         process = QProcess(self)
# #         process.start(program)
# #         if not process.waitForStarted(3000):
# #             return []
# #         process.waitForReadyRead(5000)
# #         output = process.readAllStandardOutput().data().decode("utf-8", errors="ignore")
# #         process.kill()
# #         interfaces = []
# #         for line in output.split("\n"):
# #             match = re.match(r"(\d+): (.+)", line.strip())
# #             if match:
# #                 index, desc = match.groups()
# #                 interfaces.append((int(index), desc.strip()))
# #         return interfaces

# #     def start_sniffer_process(self, interface_index: int, folder_path: str) -> None:
# #         if self.process:
# #             self.process.kill()
# #             self.process.waitForFinished(1000)
# #         self.process = QProcess(self)
# #         self.process.setProcessChannelMode(QProcess.MergedChannels)
# #         self.process.readyReadStandardOutput.connect(self.read_sniffer_output)
# #         self.process.finished.connect(self.process_finished)
# #         program = r"C:\Users\kmit\Desktop\name\x64\Debug\name.exe"
# #         if not os.path.exists(program):
# #             QMessageBox.critical(self, "Error", "name.exe not found!")
# #             self.stop_capture()
# #             return
# #         self.process.start(program)
# #         if not self.process.waitForStarted(3000):
# #             QMessageBox.critical(self, "Error", "Failed to start name.exe!")
# #             self.stop_capture()
# #             return
# #         self.process.write(f"{interface_index}\n".encode())
# #         self.process.write(f"{folder_path}\n".encode())
# #         self.status_bar.showMessage("Capturing packets...")

# #     def read_sniffer_output(self) -> None:
# #         if self.is_paused:
# #             return
# #         output = self.process.readAllStandardOutput().data().decode("utf-8", errors="ignore")
# #         if not output:
# #             return
# #         packet_blocks = output.split("===== Packet #")[1:] if "===== Packet #" in output else []
# #         for block in packet_blocks:
# #             lines = block.strip().split("\n")
# #             if not lines:
# #                 continue

# #             packet_number_match = re.match(r"(\d+)", lines[0])
# #             if not packet_number_match:
# #                 continue
# #             packet_number = packet_number_match.group(1)

# #             timestamp = ""
# #             src_ip = "Unknown"
# #             dst_ip = "Unknown"
# #             protocol = "Unknown"
# #             size = "0"
# #             details = {}
# #             current_section = None

# #             for line in lines:
# #                 line = line.strip()
# #                 if not line:
# #                     continue

# #                 # Detect section headers
# #                 section_match = re.match(r'\[(.*?)\]', line)
# #                 if section_match:
# #                     current_section = section_match.group(1)
# #                     details[current_section] = []
# #                     continue

# #                 # Parse specific fields
# #                 if current_section == "Timing Information":
# #                     if line.startswith("Timestamp: "):
# #                         timestamp = line.split("Timestamp: ", 1)[1].strip()
# #                     elif line.startswith("Captured Length: "):
# #                         size = line.split("Captured Length: ", 1)[1].strip().split()[0]
# #                 elif current_section == "Layer 3 - Network":
# #                     if line.startswith("Source IP: "):
# #                         src_ip = line.split("Source IP: ", 1)[1].strip()
# #                     elif line.startswith("Destination IP: "):
# #                         dst_ip = line.split("Destination IP: ", 1)[1].strip()
# #                     elif line.startswith("Protocol: "):
# #                         protocol = line.split("Protocol: ", 1)[1].strip()
# #                 elif current_section:
# #                     details[current_section].append(line)

# #             # Convert lists to strings for each section
# #             for section in details:
# #                 details[section] = "\n".join([l for l in details[section] if l.strip()])

# #             if timestamp:
# #                 packet = Packet(packet_number, timestamp, src_ip, dst_ip, protocol, size, details)
# #                 self.packets.append(packet)
# #                 self.log_packet_to_file(packet)
# #                 self.log_crud_operation("CAPTURE", f"Packet #{packet_number} captured")

# #         self.update_tables()

# #     def log_packet_to_file(self, packet: Packet) -> None:
# #         log_dir = r"C:\Logs"
# #         log_file = os.path.join(log_dir, "file_operations.log")
# #         if not os.path.exists(log_dir):
# #             os.makedirs(log_dir)
# #         with open(log_file, "a", encoding="utf-8") as f:
# #             f.write(f"===== Packet #{packet.number} =====\n")
# #             for section, content in packet.details.items():
# #                 f.write(f"[{section}]\n{content}\n\n")
# #             f.write("\n")

# #     def process_finished(self) -> None:
# #         if self.process:
# #             self.process = None
# #         self.stop_capture_btn.setVisible(False)
# #         self.pause_resume_btn.setVisible(False)
# #         self.progress_bar.setVisible(False)
# #         self.status_bar.showMessage("Packet capture stopped")

# #     def stop_capture(self) -> None:
# #         if self.process:
# #             self.process.write(b"\n")
# #             self.process.kill()
# #             self.process.waitForFinished(1000)
# #             self.process = None
# #         self.stop_capture_btn.setVisible(False)
# #         self.pause_resume_btn.setVisible(False)
# #         self.progress_bar.setVisible(False)
# #         if self.observer:
# #             self.observer.stop()
# #             self.observer.join()
# #         self.status_bar.showMessage("Packet capture stopped")
# #         self.animate_button(self.stop_capture_btn)

# #     def toggle_pause_resume(self) -> None:
# #         self.is_paused = not self.is_paused
# #         self.pause_resume_btn.setText("Resume" if self.is_paused else "Pause")
# #         self.pause_resume_btn.setIcon(QIcon.fromTheme("media-playback-start" if self.is_paused else "media-playback-pause"))
# #         self.status_bar.showMessage(f"Packet capture {'paused' if self.is_paused else 'resumed'} ({len(self.packets)} packets)")
# #         self.animate_button(self.pause_resume_btn)

# #     def show_packet_details(self, index) -> None:
# #         row = index.row()
# #         if row < 0 or row >= len(self.packets):
# #             return
# #         packet = self.packets[row]
# #         self.details_text.clear()
# #         self.layer_tree.clear()

# #         # Define the order of sections to display
# #         section_order = [
# #             "Timing Information",
# #             "Layer 1 - Physical",
# #             "Layer 2 - Data Link",
# #             "Payload",
# #             "Layer 3 - Network",
# #             "Layer 4 - Transport",
# #             "Layer 5 - Session",
# #             "Layer 6 - Presentation",
# #             "Layer 7 - Application"
# #         ]

# #         if self.layer_details_visible:
# #             self.layer_tree.setVisible(True)
# #             self.details_text.setVisible(False)
# #             for section in section_order:
# #                 if section in packet.details:
# #                     content = packet.details[section]
# #                     section_item = QTreeWidgetItem(self.layer_tree, [section])
# #                     section_item.setExpanded(True)
# #                     lines = content.split("\n")
# #                     for line in lines:
# #                         if line.strip():
# #                             QTreeWidgetItem(section_item, [line.strip()])
# #         else:
# #             self.layer_tree.setVisible(False)
# #             self.details_text.setVisible(True)
# #             full_details = ""
# #             for section in section_order:
# #                 if section in packet.details:
# #                     content = packet.details[section]
# #                     full_details += f"[{section}]\n{content}\n\n"
# #             self.details_text.setText(full_details.strip())

# #     def toggle_layer_details(self, checked: bool) -> None:
# #         self.layer_details_visible = checked
# #         if self.layer_details_visible:
# #             self.layer_tree.setVisible(True)
# #             self.details_text.setVisible(False)
# #         else:
# #             self.layer_tree.setVisible(False)
# #             self.details_text.setVisible(True)
# #         self.show_packet_details(self.packet_view.currentIndex())

# #     def show_file_log_details(self, index) -> None:
# #         row = index.row()
# #         if row < len(self.file_logs):
# #             log = self.file_logs[row]
# #             self.details_text.setText(log.details)
# #             self.layer_tree.setVisible(False)

# #     def show_date_dialog(self) -> None:
# #         dialog = DateSelectionDialog(self)
# #         if dialog.exec_() == QDialog.Accepted:
# #             self.apply_date_filter(dialog.selected_date())

# #     def perform_search(self) -> None:
# #         search_text = self.search_input.text().lower()
# #         if not search_text:
# #             self.load_stored_logs()
# #             self.update_tables()
# #             return
# #         filtered_packets = []
# #         filtered_logs = []
# #         for packet in self.packets:
# #             if any(search_text in str(val).lower() for val in [packet.timestamp, packet.src_ip, packet.dst_ip, packet.protocol, packet.size, *packet.details.values()]):
# #                 filtered_packets.append(packet)
# #         for log in self.file_logs:
# #             if any(search_text in str(val).lower() for val in [log.timestamp, log.operation, log.file_path, log.details]):
# #                 filtered_logs.append(log)
# #         self.packets = filtered_packets
# #         self.file_logs = filtered_logs
# #         self.update_tables()
# #         self.status_bar.showMessage(f"Search results for '{search_text}' ({len(self.packets)} packets, {len(self.file_logs)} logs)")

# #     def clear_table(self) -> None:
# #         self.packets.clear()
# #         self.file_logs.clear()
# #         self.update_tables()
# #         self.details_text.clear()
# #         self.layer_tree.clear()
# #         self.status_bar.showMessage("Table cleared")
# #         self.animate_button(self.clear_table_btn)

# #     def save_capture(self) -> None:
# #         if not self.packets:
# #             QMessageBox.warning(self, "Warning", "No packets to save!")
# #             return
# #         file_name, _ = QFileDialog.getSaveFileName(self, "Save Capture", "capture.txt", "Text Files (*.txt)")
# #         if file_name:
# #             with open(file_name, "w", encoding="utf-8") as f:
# #                 for packet in self.packets:
# #                     f.write(f"===== Packet #{packet.number} =====\n")
# #                     for section, content in packet.details.items():
# #                         f.write(f"[{section}]\n{content}\n\n")
# #                     f.write("\n")
# #             QMessageBox.information(self, "Success", f"Capture saved to {file_name}")
# #             self.status_bar.showMessage("Capture saved")

# #     def show_about(self) -> None:
# #         QMessageBox.information(self, "About", "Advanced Network Monitor\nVersion 1.4\nA professional packet sniffer and log viewer.\nDeveloped with PyQt5.")
# #         self.status_bar.showMessage("About displayed")

# #     def delete_stored_traffic(self) -> None:
# #         log_files = [r"C:\Logs\file_operations.log"]
# #         try:
# #             for log_file in log_files:
# #                 if os.path.exists(log_file):
# #                     with open(log_file, "w", encoding="utf-8") as f:
# #                         f.write("")
# #             self.packets.clear()
# #             self.file_logs.clear()
# #             self.update_tables()
# #             QMessageBox.information(self, "Success", "Stored traffic logs deleted.")
# #             self.status_bar.showMessage("Stored traffic logs deleted")
# #             self.animate_button(self.delete_logs_btn)
# #         except Exception as e:
# #             QMessageBox.warning(self, "Error", f"Failed to delete logs: {str(e)}")

# #     def start_file_monitoring(self, folder_path: str) -> None:
# #         if self.observer:
# #             self.observer.stop()
# #             self.observer.join()
# #         event_handler = FileMonitor(folder_path, self)
# #         self.observer = Observer()
# #         self.observer.schedule(event_handler, folder_path, recursive=True)
# #         self.observer.start()

# #     def log_crud_operation(self, operation: str, details: str) -> None:
# #         timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
# #         color = QColor(0, 0, 0)
# #         log = FileLog(timestamp, operation, "Network Capture", details, color)
# #         log_dir = r"C:\Logs"
# #         if not os.path.exists(log_dir):
# #             os.makedirs(log_dir)
# #         with open(os.path.join(log_dir, "file_operations.log"), "a", encoding="utf-8") as f:
# #             f.write(f"[{timestamp}] {operation}: {details}\n")
# #         self.file_logs.append(log)
# #         self.file_log_model.layoutChanged.emit()

# #     def closeEvent(self, event) -> None:
# #         if self.process:
# #             self.process.write(b"\n")
# #             self.process.kill()
# #             self.process.waitForFinished(1000)
# #             self.process = None
# #         if self.observer:
# #             self.observer.stop()
# #             self.observer.join()
# #         self.update_timer.stop()
# #         self.search_timer.stop()
# #         self.reload_timer.stop()
# #         event.accept()

# # # Application Entry Point
# # if __name__ == "__main__":
# #     QApplication.setAttribute(Qt.AA_EnableHighDpiScaling)
# #     QApplication.setAttribute(Qt.AA_UseHighDpiPixmaps)
# #     app = QApplication(sys.argv)
# #     window = NetworkMonitor()
# #     window.show()
# #     sys.exit(app.exec_())
# import sys
# import os
# import re
# import time
# from datetime import datetime
# from PyQt5.QtWidgets import (
#     QApplication, QMainWindow, QTableView, QTextEdit, QVBoxLayout, QHBoxLayout,
#     QWidget, QPushButton, QLabel, QMessageBox, QInputDialog, QHeaderView,
#     QSplitter, QStatusBar, QLineEdit, QComboBox, QDialog, QDialogButtonBox,
#     QMenuBar, QMenu, QAction, QToolBar, QToolButton, QTabWidget, QProgressBar,
#     QFileDialog, QDateEdit, QTreeWidget, QTreeWidgetItem
# )
# from PyQt5.QtCore import Qt, QProcess, QTimer, QPropertyAnimation, QEasingCurve, QAbstractTableModel, QSize, QDate
# from PyQt5.QtGui import QFont, QIcon, QPalette, QColor, QTextCharFormat, QSyntaxHighlighter
# from watchdog.observers import Observer
# from watchdog.events import FileSystemEventHandler

# # Packet Highlighter for Syntax Highlighting in Details View
# class PacketHighlighter(QSyntaxHighlighter):
#     def __init__(self, parent=None):
#         super().__init__(parent)
#         self.highlighting_rules = []
#         # IP Address
#         ip_format = QTextCharFormat()
#         ip_format.setForeground(QColor("#1E90FF"))
#         ip_format.setFontWeight(QFont.Bold)
#         self.highlighting_rules.append((r'\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\b', ip_format))
#         # Protocols
#         protocol_format = QTextCharFormat()
#         protocol_format.setForeground(QColor("#32CD32"))
#         protocol_format.setFontWeight(QFont.Bold)
#         self.highlighting_rules.append((r'\b(TCP|UDP|ICMP|ARP|IPv6|HTTP|TLS|SMTP|DNS)\b', protocol_format))
#         # Ports
#         port_format = QTextCharFormat()
#         port_format.setForeground(QColor("#FF4500"))
#         self.highlighting_rules.append((r'\bPort: \d+\b', port_format))
#         # MAC Address
#         mac_format = QTextCharFormat()
#         mac_format.setForeground(QColor("#9932CC"))
#         self.highlighting_rules.append((r'\b[0-9A-Fa-f]{2}(:[0-9A-Fa-f]{2}){5}\b', mac_format))
#         # Timestamp
#         time_format = QTextCharFormat()
#         time_format.setForeground(QColor("#FF69B4"))
#         self.highlighting_rules.append((r'\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d+', time_format))
#         # Application Data
#         app_data_format = QTextCharFormat()
#         app_data_format.setForeground(QColor("#FFD700"))
#         self.highlighting_rules.append((r'Application Data: .+', app_data_format))
#         # Payload Hex + ASCII
#         payload_format = QTextCharFormat()
#         payload_format.setForeground(QColor("#00CED1"))
#         self.highlighting_rules.append((r'Payload \(Hex \+ ASCII\):[\s\S]+?(?=\n\n|\Z)', payload_format))
#         # Delta Time
#         delta_time_format = QTextCharFormat()
#         delta_time_format.setForeground(QColor("#FFA500"))
#         self.highlighting_rules.append((r'Delta Time: \d+\.\d+ seconds', delta_time_format))

#     def highlightBlock(self, text: str) -> None:
#         for pattern, format in self.highlighting_rules:
#             for match in re.finditer(pattern, text, re.MULTILINE):
#                 start, end = match.start(), match.end()
#                 self.setFormat(start, end - start, format)

# # Packet Class with 7 Layers and Payload
# class Packet:
#     def __init__(self, number: str, timestamp: str, src_ip: str, dst_ip: str, protocol: str, size: str, delta_time: str, details: dict):
#         self.number = number
#         self.timestamp = timestamp
#         self.src_ip = src_ip
#         self.dst_ip = dst_ip
#         self.protocol = protocol
#         self.size = size
#         self.delta_time = delta_time  # New field for delta time
#         self.details = details  # Dictionary for sections, layers, and payload

# # File Log Class
# class FileLog:
#     def __init__(self, timestamp: str, operation: str, file_path: str, details: str, color: QColor = QColor(0, 0, 0)):
#         self.timestamp = timestamp
#         self.operation = operation
#         self.file_path = file_path
#         self.details = details
#         self.color = color

# # Packet Table Model
# class PacketTableModel(QAbstractTableModel):
#     def __init__(self, packets: list, parent=None):
#         super().__init__(parent)
#         self.packets = packets

#     def rowCount(self, index) -> int:
#         return len(self.packets)

#     def columnCount(self, index) -> int:
#         return 7  # Increased to 7 columns to include Delta Time

#     def data(self, index, role: int):
#         if not index.isValid():
#             return None
#         if role == Qt.DisplayRole:
#             packet = self.packets[index.row()]
#             return [packet.number, packet.timestamp, packet.src_ip, packet.dst_ip, 
#                    packet.protocol or "Unknown", packet.size or "0", 
#                    packet.delta_time or "0.000000"][index.column()]
#         if role == Qt.BackgroundRole:
#             packet = self.packets[index.row()]
#             color = QColor(255, 255, 255)
#             if packet.protocol == "TCP":
#                 color = QColor(220, 230, 255)
#             elif packet.protocol == "UDP":
#                 color = QColor(220, 255, 220)
#             elif packet.protocol == "ICMP":
#                 color = QColor(255, 220, 220)
#             elif packet.protocol == "ARP":
#                 color = QColor(240, 220, 255)
#             elif packet.protocol == "IPv6":
#                 color = QColor(255, 230, 210)
#             elif packet.protocol == "112":
#                 color = QColor(230, 210, 255)
#             elif packet.protocol == "None":
#                 color = QColor(245, 245, 220)
#             return color
#         return None

#     def headerData(self, section: int, orientation: Qt.Orientation, role: int):
#         if role != Qt.DisplayRole:
#             return None
#         headers = ["Packet #", "Timestamp", "Source IP", "Destination IP", "Protocol", "Size", "Delta Time"]
#         return headers[section] if orientation == Qt.Horizontal else str(section + 1)

# # File Log Table Model
# class FileLogTableModel(QAbstractTableModel):
#     def __init__(self, logs: list, parent=None):
#         super().__init__(parent)
#         self.logs = logs

#     def rowCount(self, index) -> int:
#         return len(self.logs)

#     def columnCount(self, index) -> int:
#         return 4

#     def data(self, index, role: int):
#         if not index.isValid():
#             return None
#         if role == Qt.DisplayRole:
#             log = self.logs[index.row()]
#             return [log.timestamp, log.operation, log.file_path, log.details][index.column()]
#         if role == Qt.BackgroundRole:
#             log = self.logs[index.row()]
#             color = QColor(255, 255, 255)
#             if log.operation == "CREATE":
#                 color = QColor(220, 255, 220)
#             elif log.operation == "UPDATE":
#                 color = QColor(255, 255, 200)
#             elif log.operation == "DELETE":
#                 color = QColor(255, 220, 220)
#             elif log.operation == "MOVE":
#                 color = QColor(220, 220, 255)
#             return color
#         if role == Qt.ForegroundRole:
#             log = self.logs[index.row()]
#             return log.color
#         return None

#     def headerData(self, section: int, orientation: Qt.Orientation, role: int):
#         if role != Qt.DisplayRole:
#             return None
#         headers = ["Timestamp", "Operation", "File Path", "Details"]
#         return headers[section] if orientation == Qt.Horizontal else str(section + 1)

# # File Monitor for Watching Shared Folder
# class FileMonitor(FileSystemEventHandler):
#     def __init__(self, folder_path: str, parent):
#         super().__init__()
#         self.folder_path = folder_path
#         self.parent = parent

#     def on_created(self, event) -> None:
#         if not event.is_directory:
#             self.log_event("CREATE", event.src_path, f"File created: {event.src_path}")

#     def on_modified(self, event) -> None:
#         if not event.is_directory:
#             self.log_event("UPDATE", event.src_path, f"File modified: {event.src_path}")

#     def on_deleted(self, event) -> None:
#         if not event.is_directory:
#             self.log_event("DELETE", event.src_path, f"File deleted: {event.src_path}")

#     def on_moved(self, event) -> None:
#         if not event.is_directory:
#             self.log_event("MOVE", event.dest_path, f"File moved from {event.src_path} to {event.dest_path}")

#     def log_event(self, operation: str, file_path: str, details: str) -> None:
#         timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
#         color = QColor(0, 0, 0)
#         log = FileLog(timestamp, operation, file_path, details, color)
#         log_dir = r"C:\Logs"
#         if not os.path.exists(log_dir):
#             os.makedirs(log_dir)
#         with open(os.path.join(log_dir, "file_operations.log"), "a", encoding="utf-8") as f:
#             f.write(f"[{timestamp}] {operation}: {details}\n")
#         self.parent.file_logs.append(log)
#         self.parent.file_log_model.layoutChanged.emit()

# # Interface Selection Dialog
# class InterfaceDialog(QDialog):
#     def __init__(self, interfaces: list, parent=None):
#         super().__init__(parent)
#         self.setWindowTitle("Select Network Interface")
#         self.setModal(True)
#         layout = QVBoxLayout(self)
#         self.combo = QComboBox()
#         for index, desc in interfaces:
#             self.combo.addItem(f"{index}: {desc}", index)
#         layout.addWidget(self.combo)
#         buttons = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
#         buttons.accepted.connect(self.accept)
#         buttons.rejected.connect(self.reject)
#         layout.addWidget(buttons)

#     def selected_index(self) -> int:
#         return self.combo.currentData()

# # Date Selection Dialog
# class DateSelectionDialog(QDialog):
#     def __init__(self, parent=None):
#         super().__init__(parent)
#         self.setWindowTitle("Select Date")
#         self.setModal(True)
#         layout = QVBoxLayout(self)
#         self.date_edit = QDateEdit()
#         self.date_edit.setCalendarPopup(True)
#         self.date_edit.setDate(QDate.currentDate())
#         self.date_edit.setDisplayFormat("yyyy-MM-dd")
#         layout.addWidget(self.date_edit)
#         buttons = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
#         buttons.accepted.connect(self.accept)
#         buttons.rejected.connect(self.reject)
#         layout.addWidget(buttons)

#     def selected_date(self) -> str:
#         return self.date_edit.date().toString("yyyy-MM-dd")

# # Main Network Monitor Window
# class NetworkMonitor(QMainWindow):
#     def __init__(self):
#         super().__init__()
#         self.setWindowTitle("Advanced Network Monitor")
#         self.setGeometry(100, 100, 1400, 900)
#         self.packets: list[Packet] = []
#         self.file_logs: list[FileLog] = []
#         self.process = None
#         self.current_mode = None
#         self.is_paused = False
#         self.observer = None
#         self.init_ui()
#         self.apply_styles()
#         self.update_timer = QTimer(self)
#         self.update_timer.timeout.connect(self.update_tables)
#         self.update_timer.start(300)
#         self.search_timer = QTimer(self)
#         self.search_timer.setSingleShot(True)
#         self.search_timer.timeout.connect(self.perform_search)
#         self.reload_timer = QTimer(self)
#         self.reload_timer.timeout.connect(self.reload_data)
#         self.reload_timer.start(1000)
#         self.layer_details_visible = False

#     def init_ui(self) -> None:
#         # Menu Bar
#         menubar = QMenuBar(self)
#         self.setMenuBar(menubar)
#         file_menu = menubar.addMenu("File")
#         tools_menu = menubar.addMenu("Tools")
#         view_menu = menubar.addMenu("View")
#         help_menu = menubar.addMenu("Help")

#         save_action = QAction(QIcon.fromTheme("document-save"), "Save Capture", self)
#         save_action.triggered.connect(self.save_capture)
#         file_menu.addAction(save_action)

#         export_action = QAction(QIcon.fromTheme("document-export"), "Export to PCAP", self)
#         export_action.triggered.connect(lambda: QMessageBox.information(self, "Info", "Export to PCAP coming soon!"))
#         file_menu.addAction(export_action)

#         preferences_action = QAction(QIcon.fromTheme("preferences-system"), "Preferences", self)
#         preferences_action.triggered.connect(lambda: QMessageBox.information(self, "Info", "Preferences coming soon!"))
#         tools_menu.addAction(preferences_action)

#         toggle_layers_action = QAction(QIcon.fromTheme("view-list-tree"), "Toggle Layer Details", self, checkable=True)
#         toggle_layers_action.toggled.connect(self.toggle_layer_details)
#         view_menu.addAction(toggle_layers_action)

#         about_action = QAction(QIcon.fromTheme("help-about"), "About", self)
#         about_action.triggered.connect(self.show_about)
#         help_menu.addAction(about_action)

#         # Toolbar
#         toolbar = QToolBar("Main Toolbar", self)
#         toolbar.setIconSize(QSize(32, 32))
#         toolbar.setToolButtonStyle(Qt.ToolButtonTextUnderIcon)
#         self.addToolBar(Qt.TopToolBarArea, toolbar)
#         self.search_input = QLineEdit()
#         self.search_input.setPlaceholderText("Search packets/logs...")
#         self.search_input.textChanged.connect(lambda: self.search_timer.start(300))
#         toolbar.addWidget(self.search_input)
#         search_btn = QToolButton()
#         search_btn.setIcon(QIcon.fromTheme("system-search"))
#         search_btn.setToolTip("Search")
#         search_btn.clicked.connect(self.perform_search)
#         toolbar.addWidget(search_btn)

#         # Main Layout
#         main_widget = QWidget()
#         self.setCentralWidget(main_widget)
#         main_layout = QVBoxLayout(main_widget)
#         main_layout.setContentsMargins(20, 20, 20, 20)
#         main_layout.setSpacing(15)

#         # Header
#         header = QLabel("Advanced Network Monitor")
#         header.setFont(QFont("Segoe UI", 28, QFont.Bold))
#         header.setAlignment(Qt.AlignCenter)
#         main_layout.addWidget(header)

#         # Tabs
#         self.tab_widget = QTabWidget()
#         main_layout.addWidget(self.tab_widget)

#         # Packet Tab
#         packet_tab = QWidget()
#         packet_tab_layout = QVBoxLayout(packet_tab)

#         # Control Panel
#         control_panel = QWidget()
#         control_layout = QHBoxLayout(control_panel)
#         control_layout.setAlignment(Qt.AlignCenter)
#         control_layout.setSpacing(20)

#         self.stored_logs_btn = QPushButton(QIcon.fromTheme("document-open"), "Stored Logs")
#         self.stored_logs_btn.setFixedSize(180, 50)
#         self.stored_logs_btn.clicked.connect(self.show_stored_logs)
#         self.stored_logs_btn.setToolTip("View stored packet logs")
#         control_layout.addWidget(self.stored_logs_btn)

#         self.current_traffic_btn = QPushButton(QIcon.fromTheme("network-connect"), "Current Traffic")
#         self.current_traffic_btn.setFixedSize(180, 50)
#         self.current_traffic_btn.clicked.connect(self.show_current_traffic)
#         self.current_traffic_btn.setToolTip("Capture live traffic")
#         control_layout.addWidget(self.current_traffic_btn)

#         self.delete_logs_btn = QPushButton(QIcon.fromTheme("edit-delete"), "Delete Logs")
#         self.delete_logs_btn.setFixedSize(180, 50)
#         self.delete_logs_btn.clicked.connect(self.delete_stored_traffic)
#         self.delete_logs_btn.setToolTip("Delete stored traffic")
#         control_layout.addWidget(self.delete_logs_btn)

#         self.stop_capture_btn = QPushButton(QIcon.fromTheme("media-playback-stop"), "Stop Capture")
#         self.stop_capture_btn.setFixedSize(180, 50)
#         self.stop_capture_btn.clicked.connect(self.stop_capture)
#         self.stop_capture_btn.setVisible(False)
#         self.stop_capture_btn.setToolTip("Stop capturing traffic")
#         control_layout.addWidget(self.stop_capture_btn)

#         self.pause_resume_btn = QPushButton(QIcon.fromTheme("media-playback-pause"), "Pause/Resume")
#         self.pause_resume_btn.setFixedSize(180, 50)
#         self.pause_resume_btn.clicked.connect(self.toggle_pause_resume)
#         self.pause_resume_btn.setVisible(False)
#         self.pause_resume_btn.setToolTip("Pause/resume capture")
#         control_layout.addWidget(self.pause_resume_btn)

#         self.clear_table_btn = QPushButton(QIcon.fromTheme("edit-clear"), "Clear Table")
#         self.clear_table_btn.setFixedSize(180, 50)
#         self.clear_table_btn.clicked.connect(self.clear_table)
#         self.clear_table_btn.setToolTip("Clear the table")
#         control_layout.addWidget(self.clear_table_btn)

#         packet_tab_layout.addWidget(control_panel)

#         # Filter Panel
#         filter_panel = QWidget()
#         filter_layout = QHBoxLayout(filter_panel)
#         filter_label = QLabel("Filter by Date:")
#         filter_label.setFont(QFont("Segoe UI", 12))
#         self.date_filter_btn = QPushButton("Select Date")
#         self.date_filter_btn.setFixedSize(100, 30)
#         self.date_filter_btn.clicked.connect(self.show_date_dialog)
#         filter_layout.addWidget(filter_label)
#         filter_layout.addWidget(self.date_filter_btn)
#         packet_tab_layout.addWidget(filter_panel)

#         # Splitter for Packets and Details
#         splitter = QSplitter(Qt.Vertical)
#         self.packet_view = QTableView()
#         self.packet_model = PacketTableModel(self.packets, self)
#         self.packet_view.setModel(self.packet_model)
#         self.packet_view.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
#         self.packet_view.setSelectionMode(QTableView.SingleSelection)
#         self.packet_view.setSortingEnabled(True)
#         self.packet_view.clicked.connect(self.show_packet_details)
#         splitter.addWidget(self.packet_view)

#         self.details_widget = QWidget()
#         details_layout = QVBoxLayout(self.details_widget)
#         self.layer_tree = QTreeWidget()
#         self.layer_tree.setHeaderHidden(True)
#         self.layer_tree.setVisible(False)
#         details_layout.addWidget(self.layer_tree)
#         self.details_text = QTextEdit()
#         self.details_text.setReadOnly(True)
#         self.details_text.setFont(QFont("Consolas", 12))
#         self.highlighter = PacketHighlighter(self.details_text.document())
#         details_layout.addWidget(self.details_text)
#         splitter.addWidget(self.details_widget)
#         splitter.setSizes([600, 300])
#         packet_tab_layout.addWidget(splitter)

#         # File Log Tab
#         file_log_tab = QWidget()
#         file_log_tab_layout = QVBoxLayout(file_log_tab)
#         self.file_log_view = QTableView()
#         self.file_log_model = FileLogTableModel(self.file_logs, self)
#         self.file_log_view.setModel(self.file_log_model)
#         self.file_log_view.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
#         self.file_log_view.setSelectionMode(QTableView.SingleSelection)
#         self.file_log_view.setSortingEnabled(True)
#         self.file_log_view.clicked.connect(self.show_file_log_details)
#         file_log_tab_layout.addWidget(self.file_log_view)

#         self.tab_widget.addTab(packet_tab, QIcon.fromTheme("network-wired"), "Packets")
#         self.tab_widget.addTab(file_log_tab, QIcon.fromTheme("folder"), "File Logs")

#         # Status Bar
#         self.status_bar = QStatusBar()
#         self.setStatusBar(self.status_bar)
#         self.progress_bar = QProgressBar()
#         self.progress_bar.setVisible(False)
#         self.status_bar.addPermanentWidget(self.progress_bar)

#     def apply_styles(self) -> None:
#         palette = QPalette()
#         palette.setColor(QPalette.Window, QColor(240, 240, 245))
#         palette.setColor(QPalette.WindowText, QColor(30, 30, 30))
#         palette.setColor(QPalette.Base, QColor(255, 255, 255))
#         palette.setColor(QPalette.AlternateBase, QColor(245, 245, 250))
#         palette.setColor(QPalette.Text, QColor(30, 30, 30))
#         palette.setColor(QPalette.Button, QColor(220, 220, 220))
#         palette.setColor(QPalette.ButtonText, QColor(30, 30, 30))
#         palette.setColor(QPalette.Highlight, QColor(0, 122, 255))
#         palette.setColor(QPalette.HighlightedText, QColor(255, 255, 255))
#         QApplication.setPalette(palette)

#         self.setStyleSheet("""
#             QMainWindow {
#                 background-color: #F0F0F5;
#             }
#             QLabel {
#                 color: #1E1E1E;
#                 font-family: 'Segoe UI';
#             }
#             QPushButton {
#                 background-color: #E6E6E6;
#                 border: 1px solid #B0B0B0;
#                 border-radius: 8px;
#                 padding: 10px;
#                 font-family: 'Segoe UI';
#                 font-size: 14px;
#                 font-weight: 600;
#                 color: #1E1E1E;
#             }
#             QPushButton:hover {
#                 background-color: #D6D6D6;
#                 border: 1px solid #A0A0A0;
#             }
#             QPushButton:pressed {
#                 background-color: #C6C6C6;
#             }
#             QTableView {
#                 background-color: #FFFFFF;
#                 color: #1E1E1E;
#                 gridline-color: #E0E0E0;
#                 selection-background-color: #007AFF;
#                 selection-color: #FFFFFF;
#                 font-family: 'Segoe UI';
#                 font-size: 13px;
#                 border: 1px solid #D0D0D0;
#                 border-radius: 5px;
#             }
#             QHeaderView::section {
#                 background-color: #F5F5F5;
#                 color: #1E1E1E;
#                 padding: 8px;
#                 border: 1px solid #D0D0D0;
#                 font-family: 'Segoe UI';
#                 font-size: 13px;
#                 font-weight: 600;
#             }
#             QTextEdit {
#                 background-color: #FFFFFF;
#                 color: #1E1E1E;
#                 border: 1px solid #D0D0D0;
#                 border-radius: 5px;
#                 font-family: 'Consolas';
#                 font-size: 13px;
#             }
#             QLineEdit {
#                 background-color: #FFFFFF;
#                 color: #1E1E1E;
#                 border: 1px solid #D0D0D0;
#                 border-radius: 5px;
#                 padding: 6px;
#                 font-family: 'Segoe UI';
#                 font-size: 13px;
#             }
#             QLineEdit:focus {
#                 border: 2px solid #007AFF;
#             }
#             QSplitter::handle {
#                 background-color: #D0D0D0;
#                 width: 5px;
#             }
#             QStatusBar {
#                 background-color: #F5F5F5;
#                 color: #1E1E1E;
#                 font-family: 'Segoe UI';
#                 font-size: 13px;
#             }
#             QTabWidget::pane {
#                 border: 1px solid #D0D0D0;
#                 border-radius: 5px;
#             }
#             QTabBar::tab {
#                 background-color: #E6E6E6;
#                 color: #1E1E1E;
#                 padding: 12px;
#                 border-top-left-radius: 5px;
#                 border-top-right-radius: 5px;
#                 font-family: 'Segoe UI';
#                 font-size: 14px;
#             }
#             QTabBar::tab:selected {
#                 background-color: #FFFFFF;
#                 border-bottom: 2px solid #007AFF;
#             }
#             QProgressBar {
#                 background-color: #E6E6E6;
#                 border: 1px solid #D0D0D0;
#                 text-align: center;
#                 color: #1E1E1E;
#                 border-radius: 5px;
#             }
#             QProgressBar::chunk {
#                 background-color: #007AFF;
#                 border-radius: 5px;
#             }
#             QToolBar {
#                 background-color: #F5F5F5;
#                 border-bottom: 1px solid #D0D0D0;
#             }
#             QTreeWidget {
#                 background-color: #FFFFFF;
#                 color: #1E1E1E;
#                 border: 1px solid #D0D0D0;
#                 border-radius: 5px;
#                 font-family: 'Segoe UI';
#                 font-size: 13px;
#             }
#             QTreeWidget::item {
#                 padding: 5px;
#             }
#             QTreeWidget::item:hover {
#                 background-color: #E6F0FA;
#             }
#         """)

#     def animate_button(self, button: QPushButton) -> None:
#         animation = QPropertyAnimation(button, b"geometry")
#         animation.setDuration(200)
#         animation.setEasingCurve(QEasingCurve.OutBounce)
#         rect = button.geometry()
#         animation.setStartValue(rect)
#         animation.setKeyValueAt(0.5, rect.adjusted(5, 5, -5, -5))
#         animation.setEndValue(rect)
#         animation.start()

#     def update_tables(self) -> None:
#         self.packet_model.layoutChanged.emit()
#         self.file_log_model.layoutChanged.emit()
#         packet_scrollbar = self.packet_view.verticalScrollBar()
#         file_log_scrollbar = self.file_log_view.verticalScrollBar()
#         if packet_scrollbar.maximum() == 0 or packet_scrollbar.value() >= packet_scrollbar.maximum() - 10:
#             self.packet_view.scrollToBottom()
#         if file_log_scrollbar.maximum() == 0 or file_log_scrollbar.value() >= file_log_scrollbar.maximum() - 10:
#             self.file_log_view.scrollToBottom()

#     def reload_data(self) -> None:
#         if self.current_mode == "stored":
#             self.load_stored_logs()
#         elif self.current_mode == "current" and self.process and not self.is_paused:
#             self.read_sniffer_output()
#         self.update_tables()

#     def show_stored_logs(self) -> None:
#         self.current_mode = "stored"
#         self.packets.clear()
#         self.details_text.clear()
#         self.layer_tree.clear()
#         self.stop_capture_btn.setVisible(False)
#         self.pause_resume_btn.setVisible(False)
#         self.progress_bar.setVisible(True)
#         self.progress_bar.setRange(0, 0)
#         self.status_bar.showMessage("Loading stored logs...")
#         self.load_stored_logs()
#         self.progress_bar.setVisible(False)
#         self.status_bar.showMessage(f"Stored logs loaded ({len(self.packets)} packets)")
#         self.animate_button(self.stored_logs_btn)

#     def load_stored_logs(self) -> None:
#         log_file = r"C:\Logs\file_operations.log"
#         self.packets.clear()
#         try:
#             if not os.path.exists(log_file):
#                 QMessageBox.information(self, "Info", "No stored logs found.")
#                 return

#             with open(log_file, "r", encoding="utf-8") as f:
#                 content = f.read()

#             # Split content by packet blocks, but only include blocks starting with "===== Packet #"
#             packet_blocks = re.split(r'===== Packet #(\d+) =====', content)[1:]
#             packet_blocks = [(packet_blocks[i], packet_blocks[i + 1]) for i in range(0, len(packet_blocks) - 1, 2)]

#             for packet_number, block in packet_blocks:
#                 # Skip any non-packet content (e.g., "[2025-04-28 ...]") within the block
#                 lines = block.strip().split("\n")
#                 if not lines:
#                     continue

#                 timestamp = ""
#                 src_ip = "Unknown"
#                 dst_ip = "Unknown"
#                 protocol = "Unknown"
#                 size = "0"
#                 delta_time = "0.000000"
#                 details = {}
#                 current_section = None

#                 for line in lines:
#                     line = line.strip()
#                     if not line:
#                         continue

#                     # Skip lines that match the CRUD log format (e.g., "[2025-04-28 ...]")
#                     if re.match(r'\[\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\]', line):
#                         continue

#                     # Detect section headers
#                     section_match = re.match(r'\[(.*?)\]', line)
#                     if section_match:
#                         current_section = section_match.group(1)
#                         details[current_section] = []
#                         continue

#                     # Parse specific fields
#                     if current_section == "Timing Information":
#                         if line.startswith("Timestamp: "):
#                             timestamp = line.split("Timestamp: ", 1)[1].strip()
#                         elif line.startswith("Captured Length: "):
#                             size = line.split("Captured Length: ", 1)[1].strip().split()[0]
#                         elif line.startswith("Delta Time: "):
#                             delta_time = line.split("Delta Time: ", 1)[1].strip().split()[0]
#                     elif current_section == "Layer 3 - Network":
#                         if line.startswith("Source IP: "):
#                             src_ip = line.split("Source IP: ", 1)[1].strip()
#                         elif line.startswith("Destination IP: "):
#                             dst_ip = line.split("Destination IP: ", 1)[1].strip()
#                         elif line.startswith("Protocol: "):
#                             protocol = line.split("Protocol: ", 1)[1].strip()
#                     elif current_section:
#                         details[current_section].append(line)

#                 # Convert lists to strings for each section
#                 for section in details:
#                     details[section] = "\n".join([l for l in details[section] if l.strip()])

#                 if timestamp:
#                     packet = Packet(packet_number, timestamp, src_ip, dst_ip, protocol, size, delta_time, details)
#                     self.packets.append(packet)

#             self.packet_model.layoutChanged.emit()
#             if not self.packets:
#                 QMessageBox.information(self, "Info", "No valid packets found in the log file.")

#         except Exception as e:
#             QMessageBox.warning(self, "Error", f"Failed to load logs: {str(e)}")
#             with open(r"C:\Logs\debug.log", "a", encoding="utf-8") as f:
#                 f.write(f"[{time.strftime('%Y-%m-%d %H:%M:%S')}] Error loading logs: {str(e)}\n")

#     def apply_date_filter(self, date_str: str) -> None:
#         if not date_str:
#             return
#         try:
#             datetime.strptime(date_str, "%Y-%m-%d")
#             filtered_packets = [p for p in self.packets if p.timestamp.startswith(date_str)]
#             self.packets = filtered_packets
#             self.update_tables()
#             self.status_bar.showMessage(f"Filtered packets for {date_str} ({len(self.packets)} packets)")
#         except ValueError:
#             QMessageBox.warning(self, "Warning", "Invalid date format")

#     def show_current_traffic(self) -> None:
#         self.current_mode = "current"
#         self.packets.clear()
#         self.details_text.clear()
#         self.layer_tree.clear()
#         self.stop_capture_btn.setVisible(True)
#         self.pause_resume_btn.setVisible(True)
#         self.is_paused = False
#         self.pause_resume_btn.setText("Pause")
#         self.pause_resume_btn.setIcon(QIcon.fromTheme("media-playback-pause"))
#         self.progress_bar.setVisible(True)
#         self.progress_bar.setRange(0, 0)
#         self.status_bar.showMessage("Fetching network interfaces...")

#         interfaces = self.get_network_interfaces()
#         if not interfaces:
#             QMessageBox.critical(self, "Error", "No network interfaces found or failed to run name.exe")
#             self.stop_capture()
#             return

#         dialog = InterfaceDialog(interfaces, self)
#         if dialog.exec_() != QDialog.Accepted:
#             self.stop_capture()
#             return
#         interface_index = dialog.selected_index()

#         folder_path = QFileDialog.getExistingDirectory(self, "Select Shared Folder", os.path.expanduser("~"))
#         if not folder_path:
#             self.stop_capture()
#             return

#         self.start_file_monitoring(folder_path)
#         self.start_sniffer_process(interface_index, folder_path)
#         self.animate_button(self.current_traffic_btn)

#     def get_network_interfaces(self) -> list:
#         program = r"C:\Users\kmit\Desktop\name\x64\Debug\name.exe"
#         if not os.path.exists(program):
#             return []
#         process = QProcess(self)
#         process.start(program)
#         if not process.waitForStarted(3000):
#             return []
#         process.waitForReadyRead(5000)
#         output = process.readAllStandardOutput().data().decode("utf-8", errors="ignore")
#         process.kill()
#         interfaces = []
#         for line in output.split("\n"):
#             match = re.match(r"(\d+): (.+)", line.strip())
#             if match:
#                 index, desc = match.groups()
#                 interfaces.append((int(index), desc.strip()))
#         return interfaces

#     def start_sniffer_process(self, interface_index: int, folder_path: str) -> None:
#         if self.process:
#             self.process.kill()
#             self.process.waitForFinished(1000)
#         self.process = QProcess(self)
#         self.process.setProcessChannelMode(QProcess.MergedChannels)
#         self.process.readyReadStandardOutput.connect(self.read_sniffer_output)
#         self.process.finished.connect(self.process_finished)
#         program = r"C:\Users\kmit\Desktop\name\x64\Debug\name.exe"
#         if not os.path.exists(program):
#             QMessageBox.critical(self, "Error", "name.exe not found!")
#             self.stop_capture()
#             return
#         self.process.start(program)
#         if not self.process.waitForStarted(3000):
#             QMessageBox.critical(self, "Error", "Failed to start name.exe!")
#             self.stop_capture()
#             return
#         self.process.write(f"{interface_index}\n".encode())
#         self.process.write(f"{folder_path}\n".encode())
#         self.status_bar.showMessage("Capturing packets...")

#     def read_sniffer_output(self) -> None:
#         if self.is_paused:
#             return
#         output = self.process.readAllStandardOutput().data().decode("utf-8", errors="ignore")
#         if not output:
#             return
#         packet_blocks = output.split("===== Packet #")[1:] if "===== Packet #" in output else []
#         for block in packet_blocks:
#             lines = block.strip().split("\n")
#             if not lines:
#                 continue

#             packet_number_match = re.match(r"(\d+)", lines[0])
#             if not packet_number_match:
#                 continue
#             packet_number = packet_number_match.group(1)

#             timestamp = ""
#             src_ip = "Unknown"
#             dst_ip = "Unknown"
#             protocol = "Unknown"
#             size = "0"
#             delta_time = "0.000000"
#             details = {}
#             current_section = None

#             for line in lines:
#                 line = line.strip()
#                 if not line:
#                     continue

#                 # Detect section headers
#                 section_match = re.match(r'\[(.*?)\]', line)
#                 if section_match:
#                     current_section = section_match.group(1)
#                     details[current_section] = []
#                     continue

#                 # Parse specific fields
#                 if current_section == "Timing Information":
#                     if line.startswith("Timestamp: "):
#                         timestamp = line.split("Timestamp: ", 1)[1].strip()
#                     elif line.startswith("Captured Length: "):
#                         size = line.split("Captured Length: ", 1)[1].strip().split()[0]
#                     elif line.startswith("Delta Time: "):
#                         delta_time = line.split("Delta Time: ", 1)[1].strip().split()[0]
#                 elif current_section == "Layer 3 - Network":
#                     if line.startswith("Source IP: "):
#                         src_ip = line.split("Source IP: ", 1)[1].strip()
#                     elif line.startswith("Destination IP: "):
#                         dst_ip = line.split("Destination IP: ", 1)[1].strip()
#                     elif line.startswith("Protocol: "):
#                         protocol = line.split("Protocol: ", 1)[1].strip()
#                 elif current_section:
#                     details[current_section].append(line)

#             # Convert lists to strings for each section
#             for section in details:
#                 details[section] = "\n".join([l for l in details[section] if l.strip()])

#             if timestamp:
#                 packet = Packet(packet_number, timestamp, src_ip, dst_ip, protocol, size, delta_time, details)
#                 self.packets.append(packet)
#                 self.log_packet_to_file(packet)
#                 self.log_crud_operation("CAPTURE", f"Packet #{packet_number} captured")

#         self.update_tables()

#     def log_packet_to_file(self, packet: Packet) -> None:
#         log_dir = r"C:\Logs"
#         log_file = os.path.join(log_dir, "file_operations.log")
#         if not os.path.exists(log_dir):
#             os.makedirs(log_dir)
#         with open(log_file, "a", encoding="utf-8") as f:
#             f.write(f"===== Packet #{packet.number} =====\n")
#             for section, content in packet.details.items():
#                 f.write(f"[{section}]\n{content}\n\n")
#             f.write("\n")

#     def process_finished(self) -> None:
#         if self.process:
#             self.process = None
#         self.stop_capture_btn.setVisible(False)
#         self.pause_resume_btn.setVisible(False)
#         self.progress_bar.setVisible(False)
#         self.status_bar.showMessage("Packet capture stopped")

#     def stop_capture(self) -> None:
#         if self.process:
#             self.process.write(b"\n")
#             self.process.kill()
#             self.process.waitForFinished(1000)
#             self.process = None
#         self.stop_capture_btn.setVisible(False)
#         self.pause_resume_btn.setVisible(False)
#         self.progress_bar.setVisible(False)
#         if self.observer:
#             self.observer.stop()
#             self.observer.join()
#         self.status_bar.showMessage("Packet capture stopped")
#         self.animate_button(self.stop_capture_btn)

#     def toggle_pause_resume(self) -> None:
#         self.is_paused = not self.is_paused
#         self.pause_resume_btn.setText("Resume" if self.is_paused else "Pause")
#         self.pause_resume_btn.setIcon(QIcon.fromTheme("media-playback-start" if self.is_paused else "media-playback-pause"))
#         self.status_bar.showMessage(f"Packet capture {'paused' if self.is_paused else 'resumed'} ({len(self.packets)} packets)")
#         self.animate_button(self.pause_resume_btn)

#     def show_packet_details(self, index) -> None:
#         row = index.row()
#         if row < 0 or row >= len(self.packets):
#             return
#         packet = self.packets[row]
#         self.details_text.clear()
#         self.layer_tree.clear()

#         # Define the order of sections to display
#         section_order = [
#             "Timing Information",
#             "Layer 1 - Physical",
#             "Layer 2 - Data Link",
#             "Payload",
#             "Layer 3 - Network",
#             "Layer 4 - Transport",
#             "Layer 5 - Session",
#             "Layer 6 - Presentation",
#             "Layer 7 - Application"
#         ]

#         if self.layer_details_visible:
#             self.layer_tree.setVisible(True)
#             self.details_text.setVisible(False)
#             for section in section_order:
#                 if section in packet.details:
#                     content = packet.details[section]
#                     section_item = QTreeWidgetItem(self.layer_tree, [section])
#                     section_item.setExpanded(True)
#                     lines = content.split("\n")
#                     for line in lines:
#                         if line.strip():
#                             QTreeWidgetItem(section_item, [line.strip()])
#         else:
#             self.layer_tree.setVisible(False)
#             self.details_text.setVisible(True)
#             full_details = ""
#             for section in section_order:
#                 if section in packet.details:
#                     content = packet.details[section]
#                     full_details += f"[{section}]\n{content}\n\n"
#             self.details_text.setText(full_details.strip())

#     def toggle_layer_details(self, checked: bool) -> None:
#         self.layer_details_visible = checked
#         if self.layer_details_visible:
#             self.layer_tree.setVisible(True)
#             self.details_text.setVisible(False)
#         else:
#             self.layer_tree.setVisible(False)
#             self.details_text.setVisible(True)
#         self.show_packet_details(self.packet_view.currentIndex())

#     def show_file_log_details(self, index) -> None:
#         row = index.row()
#         if row < len(self.file_logs):
#             log = self.file_logs[row]
#             self.details_text.setText(log.details)
#             self.layer_tree.setVisible(False)

#     def show_date_dialog(self) -> None:
#         dialog = DateSelectionDialog(self)
#         if dialog.exec_() == QDialog.Accepted:
#             self.apply_date_filter(dialog.selected_date())

#     def perform_search(self) -> None:
#         search_text = self.search_input.text().lower()
#         if not search_text:
#             self.load_stored_logs()
#             self.update_tables()
#             return
#         filtered_packets = []
#         filtered_logs = []
#         for packet in self.packets:
#             if any(search_text in str(val).lower() for val in [packet.timestamp, packet.src_ip, packet.dst_ip, 
#                                                               packet.protocol, packet.size, packet.delta_time, 
#                                                               *packet.details.values()]):
#                 filtered_packets.append(packet)
#         for log in self.file_logs:
#             if any(search_text in str(val).lower() for val in [log.timestamp, log.operation, log.file_path, log.details]):
#                 filtered_logs.append(log)
#         self.packets = filtered_packets
#         self.file_logs = filtered_logs
#         self.update_tables()
#         self.status_bar.showMessage(f"Search results for '{search_text}' ({len(self.packets)} packets, {len(self.file_logs)} logs)")

#     def clear_table(self) -> None:
#         self.packets.clear()
#         self.file_logs.clear()
#         self.update_tables()
#         self.details_text.clear()
#         self.layer_tree.clear()
#         self.status_bar.showMessage("Table cleared")
#         self.animate_button(self.clear_table_btn)

#     def save_capture(self) -> None:
#         if not self.packets:
#             QMessageBox.warning(self, "Warning", "No packets to save!")
#             return
#         file_name, _ = QFileDialog.getSaveFileName(self, "Save Capture", "capture.txt", "Text Files (*.txt)")
#         if file_name:
#             with open(file_name, "w", encoding="utf-8") as f:
#                 for packet in self.packets:
#                     f.write(f"===== Packet #{packet.number} =====\n")
#                     for section, content in packet.details.items():
#                         f.write(f"[{section}]\n{content}\n\n")
#                     f.write("\n")
#             QMessageBox.information(self, "Success", f"Capture saved to {file_name}")
#             self.status_bar.showMessage("Capture saved")

#     def show_about(self) -> None:
#         QMessageBox.information(self, "About", "Advanced Network Monitor\nVersion 1.4\nA professional packet sniffer and log viewer.\nDeveloped with PyQt5.")
#         self.status_bar.showMessage("About displayed")

#     def delete_stored_traffic(self) -> None:
#         log_files = [r"C:\Logs\file_operations.log"]
#         try:
#             for log_file in log_files:
#                 if os.path.exists(log_file):
#                     with open(log_file, "w", encoding="utf-8") as f:
#                         f.write("")
#             self.packets.clear()
#             self.file_logs.clear()
#             self.update_tables()
#             QMessageBox.information(self, "Success", "Stored traffic logs deleted.")
#             self.status_bar.showMessage("Stored traffic logs deleted")
#             self.animate_button(self.delete_logs_btn)
#         except Exception as e:
#             QMessageBox.warning(self, "Error", f"Failed to delete logs: {str(e)}")

#     def start_file_monitoring(self, folder_path: str) -> None:
#         if self.observer:
#             self.observer.stop()
#             self.observer.join()
#         event_handler = FileMonitor(folder_path, self)
#         self.observer = Observer()
#         self.observer.schedule(event_handler, folder_path, recursive=True)
#         self.observer.start()

#     def log_crud_operation(self, operation: str, details: str) -> None:
#         timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
#         color = QColor(0, 0, 0)
#         log = FileLog(timestamp, operation, "Network Capture", details, color)
#         log_dir = r"C:\Logs"
#         if not os.path.exists(log_dir):
#             os.makedirs(log_dir)
#         with open(os.path.join(log_dir, "file_operations.log"), "a", encoding="utf-8") as f:
#             f.write(f"[{timestamp}] {operation}: {details}\n")
#         self.file_logs.append(log)
#         self.file_log_model.layoutChanged.emit()

#     def closeEvent(self, event) -> None:
#         if self.process:
#             self.process.write(b"\n")
#             self.process.kill()
#             self.process.waitForFinished(1000)
#             self.process = None
#         if self.observer:
#             self.observer.stop()
#             self.observer.join()
#         self.update_timer.stop()
#         self.search_timer.stop()
#         self.reload_timer.stop()
#         event.accept()

# # Application Entry Point
# if __name__ == "__main__":
#     QApplication.setAttribute(Qt.AA_EnableHighDpiScaling)
#     QApplication.setAttribute(Qt.AA_UseHighDpiPixmaps)
#     app = QApplication(sys.argv)
#     window = NetworkMonitor()
#     window.show()
#     sys.exit(app.exec_())

import sys
import os
import re
import time
from datetime import datetime
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QTableView, QTextEdit, QVBoxLayout, QHBoxLayout,
    QWidget, QPushButton, QLabel, QMessageBox, QInputDialog, QHeaderView,
    QSplitter, QStatusBar, QLineEdit, QComboBox, QDialog, QDialogButtonBox,
    QMenuBar, QMenu, QAction, QToolBar, QToolButton, QTabWidget, QProgressBar,
    QFileDialog, QDateEdit, QTreeWidget, QTreeWidgetItem
)
from PyQt5.QtCore import Qt, QProcess, QTimer, QPropertyAnimation, QEasingCurve, QAbstractTableModel, QSize, QDate
from PyQt5.QtGui import QFont, QIcon, QPalette, QColor, QTextCharFormat, QSyntaxHighlighter
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

# Packet Highlighter for Syntax Highlighting in Details View
class PacketHighlighter(QSyntaxHighlighter):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.highlighting_rules = []
        # IP Address
        ip_format = QTextCharFormat()
        ip_format.setForeground(QColor("#1E90FF"))
        ip_format.setFontWeight(QFont.Bold)
        self.highlighting_rules.append((r'\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\b', ip_format))
        # Protocols
        protocol_format = QTextCharFormat()
        protocol_format.setForeground(QColor("#32CD32"))
        protocol_format.setFontWeight(QFont.Bold)
        self.highlighting_rules.append((r'\b(TCP|UDP|ICMP|ARP|IPv6|HTTP|TLS|SMTP|DNS)\b', protocol_format))
        # Ports
        port_format = QTextCharFormat()
        port_format.setForeground(QColor("#FF4500"))
        self.highlighting_rules.append((r'\bPort: \d+\b', port_format))
        # MAC Address
        mac_format = QTextCharFormat()
        mac_format.setForeground(QColor("#9932CC"))
        self.highlighting_rules.append((r'\b[0-9A-Fa-f]{2}(:[0-9A-Fa-f]{2}){5}\b', mac_format))
        # Timestamp
        time_format = QTextCharFormat()
        time_format.setForeground(QColor("#FF69B4"))
        self.highlighting_rules.append((r'\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d+', time_format))
        # Application Data
        app_data_format = QTextCharFormat()
        app_data_format.setForeground(QColor("#FFD700"))
        self.highlighting_rules.append((r'Application Data: .+', app_data_format))
        # Payload Hex + ASCII
        payload_format = QTextCharFormat()
        payload_format.setForeground(QColor("#00CED1"))
        self.highlighting_rules.append((r'Payload \(Hex \+ ASCII\):[\s\S]+?(?=\n\n|\Z)', payload_format))
        # Delta Time
        delta_time_format = QTextCharFormat()
        delta_time_format.setForeground(QColor("#FFA500"))
        self.highlighting_rules.append((r'Delta Time: \d+\.\d+ seconds', delta_time_format))

    def highlightBlock(self, text: str) -> None:
        for pattern, format in self.highlighting_rules:
            for match in re.finditer(pattern, text, re.MULTILINE):
                start, end = match.start(), match.end()
                self.setFormat(start, end - start, format)

# Packet Class with 7 Layers and Payload
class Packet:
    def __init__(self, number: str, timestamp: str, src_ip: str, dst_ip: str, protocol: str, size: str, delta_time: str, details: dict):
        self.number = number
        self.timestamp = timestamp
        self.src_ip = src_ip
        self.dst_ip = dst_ip
        self.protocol = protocol
        self.size = size
        self.delta_time = delta_time  # New field for delta time
        self.details = details  # Dictionary for sections, layers, and payload

# File Log Class
class FileLog:
    def __init__(self, timestamp: str, operation: str, file_path: str, details: str, color: QColor = QColor(0, 0, 0)):
        self.timestamp = timestamp
        self.operation = operation
        self.file_path = file_path
        self.details = details
        self.color = color

# Packet Table Model
class PacketTableModel(QAbstractTableModel):
    def __init__(self, packets: list, parent=None):
        super().__init__(parent)
        self.packets = packets

    def rowCount(self, index) -> int:
        return len(self.packets)

    def columnCount(self, index) -> int:
        return 7  # Increased to 7 columns to include Delta Time

    def data(self, index, role: int):
        if not index.isValid():
            return None
        if role == Qt.DisplayRole:
            packet = self.packets[index.row()]
            return [packet.number, packet.timestamp, packet.src_ip, packet.dst_ip, 
                   packet.protocol or "Unknown", packet.size or "0", 
                   packet.delta_time or "0.000000"][index.column()]
        if role == Qt.BackgroundRole:
            packet = self.packets[index.row()]
            color = QColor(255, 255, 255)
            if packet.protocol == "TCP":
                color = QColor(220, 230, 255)
            elif packet.protocol == "UDP":
                color = QColor(220, 255, 220)
            elif packet.protocol == "ICMP":
                color = QColor(255, 220, 220)
            elif packet.protocol == "ARP":
                color = QColor(240, 220, 255)
            elif packet.protocol == "IPv6":
                color = QColor(255, 230, 210)
            elif packet.protocol == "112":
                color = QColor(230, 210, 255)
            elif packet.protocol == "None":
                color = QColor(245, 245, 220)
            return color
        return None

    def headerData(self, section: int, orientation: Qt.Orientation, role: int):
        if role != Qt.DisplayRole:
            return None
        headers = ["Packet #", "Timestamp", "Source IP", "Destination IP", "Protocol", "Size", "Delta Time"]
        return headers[section] if orientation == Qt.Horizontal else str(section + 1)

# File Log Table Model
class FileLogTableModel(QAbstractTableModel):
    def __init__(self, logs: list, parent=None):
        super().__init__(parent)
        self.logs = logs

    def rowCount(self, index) -> int:
        return len(self.logs)

    def columnCount(self, index) -> int:
        return 4

    def data(self, index, role: int):
        if not index.isValid():
            return None
        if role == Qt.DisplayRole:
            log = self.logs[index.row()]
            return [log.timestamp, log.operation, log.file_path, log.details][index.column()]
        if role == Qt.BackgroundRole:
            log = self.logs[index.row()]
            color = QColor(255, 255, 255)
            if log.operation == "CREATE":
                color = QColor(220, 255, 220)
            elif log.operation == "UPDATE":
                color = QColor(255, 255, 200)
            elif log.operation == "DELETE":
                color = QColor(255, 220, 220)
            elif log.operation == "MOVE":
                color = QColor(220, 220, 255)
            return color
        if role == Qt.ForegroundRole:
            log = self.logs[index.row()]
            return log.color
        return None

    def headerData(self, section: int, orientation: Qt.Orientation, role: int):
        if role != Qt.DisplayRole:
            return None
        headers = ["Timestamp", "Operation", "File Path", "Details"]
        return headers[section] if orientation == Qt.Horizontal else str(section + 1)

# File Monitor for Watching Shared Folder
class FileMonitor(FileSystemEventHandler):
    def __init__(self, folder_path: str, parent):
        super().__init__()
        self.folder_path = folder_path
        self.parent = parent

    def on_created(self, event) -> None:
        if not event.is_directory:
            self.log_event("CREATE", event.src_path, f"File created: {event.src_path}")

    def on_modified(self, event) -> None:
        if not event.is_directory:
            self.log_event("UPDATE", event.src_path, f"File modified: {event.src_path}")

    def on_deleted(self, event) -> None:
        if not event.is_directory:
            self.log_event("DELETE", event.src_path, f"File deleted: {event.src_path}")

    def on_moved(self, event) -> None:
        if not event.is_directory:
            self.log_event("MOVE", event.dest_path, f"File moved from {event.src_path} to {event.dest_path}")

    def log_event(self, operation: str, file_path: str, details: str) -> None:
        timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
        color = QColor(0, 0, 0)
        log = FileLog(timestamp, operation, file_path, details, color)
        log_dir = r"C:\Logs"
        if not os.path.exists(log_dir):
            os.makedirs(log_dir)
        with open(os.path.join(log_dir, "file_operations.log"), "a", encoding="utf-8") as f:
            f.write(f"[{timestamp}] {operation}: {details}\n")
        self.parent.file_logs.append(log)
        self.parent.file_log_model.layoutChanged.emit()

# Interface Selection Dialog
class InterfaceDialog(QDialog):
    def __init__(self, interfaces: list, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Select Network Interface")
        self.setModal(True)
        layout = QVBoxLayout(self)
        self.combo = QComboBox()
        for index, desc in interfaces:
            self.combo.addItem(f"{index}: {desc}", index)
        layout.addWidget(self.combo)
        buttons = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        buttons.accepted.connect(self.accept)
        buttons.rejected.connect(self.reject)
        layout.addWidget(buttons)

    def selected_index(self) -> int:
        return self.combo.currentData()

# Date Selection Dialog
class DateSelectionDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Select Date")
        self.setModal(True)
        layout = QVBoxLayout(self)
        self.date_edit = QDateEdit()
        self.date_edit.setCalendarPopup(True)
        self.date_edit.setDate(QDate.currentDate())
        self.date_edit.setDisplayFormat("yyyy-MM-dd")
        layout.addWidget(self.date_edit)
        buttons = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        buttons.accepted.connect(self.accept)
        buttons.rejected.connect(self.reject)
        layout.addWidget(buttons)

    def selected_date(self) -> str:
        return self.date_edit.date().toString("yyyy-MM-dd")

# Main Network Monitor Window
class NetworkMonitor(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Advanced Network Monitor")
        self.setGeometry(100, 100, 1400, 900)
        self.packets: list[Packet] = []
        self.file_logs: list[FileLog] = []
        self.process = None
        self.current_mode = None
        self.is_paused = False
        self.observer = None
        self.init_ui()
        self.apply_styles()
        self.update_timer = QTimer(self)
        self.update_timer.timeout.connect(self.update_tables)
        self.update_timer.start(300)
        self.search_timer = QTimer(self)
        self.search_timer.setSingleShot(True)
        self.search_timer.timeout.connect(self.perform_search)
        self.reload_timer = QTimer(self)
        self.reload_timer.timeout.connect(self.reload_data)
        self.reload_timer.start(1000)
        self.layer_details_visible = False

    def init_ui(self) -> None:
        # Menu Bar
        menubar = QMenuBar(self)
        self.setMenuBar(menubar)
        file_menu = menubar.addMenu("File")
        tools_menu = menubar.addMenu("Tools")
        view_menu = menubar.addMenu("View")
        help_menu = menubar.addMenu("Help")

        save_action = QAction(QIcon.fromTheme("document-save"), "Save Capture", self)
        save_action.triggered.connect(self.save_capture)
        file_menu.addAction(save_action)

        export_action = QAction(QIcon.fromTheme("document-export"), "Export to PCAP", self)
        export_action.triggered.connect(lambda: QMessageBox.information(self, "Info", "Export to PCAP coming soon!"))
        file_menu.addAction(export_action)

        preferences_action = QAction(QIcon.fromTheme("preferences-system"), "Preferences", self)
        preferences_action.triggered.connect(lambda: QMessageBox.information(self, "Info", "Preferences coming soon!"))
        tools_menu.addAction(preferences_action)

        toggle_layers_action = QAction(QIcon.fromTheme("view-list-tree"), "Toggle Layer Details", self, checkable=True)
        toggle_layers_action.toggled.connect(self.toggle_layer_details)
        view_menu.addAction(toggle_layers_action)

        about_action = QAction(QIcon.fromTheme("help-about"), "About", self)
        about_action.triggered.connect(self.show_about)
        help_menu.addAction(about_action)

        # Toolbar
        toolbar = QToolBar("Main Toolbar", self)
        toolbar.setIconSize(QSize(32, 32))
        toolbar.setToolButtonStyle(Qt.ToolButtonTextUnderIcon)
        self.addToolBar(Qt.TopToolBarArea, toolbar)
        self.search_input = QLineEdit()
        self.search_input.setPlaceholderText("Search packets/logs...")
        self.search_input.textChanged.connect(lambda: self.search_timer.start(300))
        toolbar.addWidget(self.search_input)
        search_btn = QToolButton()
        search_btn.setIcon(QIcon.fromTheme("system-search"))
        search_btn.setToolTip("Search")
        search_btn.clicked.connect(self.perform_search)
        toolbar.addWidget(search_btn)

        # Main Layout
        main_widget = QWidget()
        self.setCentralWidget(main_widget)
        main_layout = QVBoxLayout(main_widget)
        main_layout.setContentsMargins(20, 20, 20, 20)
        main_layout.setSpacing(15)

        # Header
        header = QLabel("Advanced Network Monitor")
        header.setFont(QFont("Segoe UI", 28, QFont.Bold))
        header.setAlignment(Qt.AlignCenter)
        main_layout.addWidget(header)

        # Tabs
        self.tab_widget = QTabWidget()
        main_layout.addWidget(self.tab_widget)

        # Packet Tab
        packet_tab = QWidget()
        packet_tab_layout = QVBoxLayout(packet_tab)

        # Control Panel
        control_panel = QWidget()
        control_layout = QHBoxLayout(control_panel)
        control_layout.setAlignment(Qt.AlignCenter)
        control_layout.setSpacing(20)

        self.stored_logs_btn = QPushButton(QIcon.fromTheme("document-open"), "Stored Logs")
        self.stored_logs_btn.setFixedSize(180, 50)
        self.stored_logs_btn.clicked.connect(self.show_stored_logs)
        self.stored_logs_btn.setToolTip("View stored packet logs")
        control_layout.addWidget(self.stored_logs_btn)

        self.current_traffic_btn = QPushButton(QIcon.fromTheme("network-connect"), "Current Traffic")
        self.current_traffic_btn.setFixedSize(180, 50)
        self.current_traffic_btn.clicked.connect(self.show_current_traffic)
        self.current_traffic_btn.setToolTip("Capture live traffic")
        control_layout.addWidget(self.current_traffic_btn)

        self.delete_logs_btn = QPushButton(QIcon.fromTheme("edit-delete"), "Delete Logs")
        self.delete_logs_btn.setFixedSize(180, 50)
        self.delete_logs_btn.clicked.connect(self.delete_stored_traffic)
        self.delete_logs_btn.setToolTip("Delete stored traffic")
        control_layout.addWidget(self.delete_logs_btn)

        self.stop_capture_btn = QPushButton(QIcon.fromTheme("media-playback-stop"), "Stop Capture")
        self.stop_capture_btn.setFixedSize(180, 50)
        self.stop_capture_btn.clicked.connect(self.stop_capture)
        self.stop_capture_btn.setVisible(False)
        self.stop_capture_btn.setToolTip("Stop capturing traffic")
        control_layout.addWidget(self.stop_capture_btn)

        self.pause_resume_btn = QPushButton(QIcon.fromTheme("media-playback-pause"), "Pause/Resume")
        self.pause_resume_btn.setFixedSize(180, 50)
        self.pause_resume_btn.clicked.connect(self.toggle_pause_resume)
        self.pause_resume_btn.setVisible(False)
        self.pause_resume_btn.setToolTip("Pause/resume capture")
        control_layout.addWidget(self.pause_resume_btn)

        self.clear_table_btn = QPushButton(QIcon.fromTheme("edit-clear"), "Clear Table")
        self.clear_table_btn.setFixedSize(180, 50)
        self.clear_table_btn.clicked.connect(self.clear_table)
        self.clear_table_btn.setToolTip("Clear the table")
        control_layout.addWidget(self.clear_table_btn)

        packet_tab_layout.addWidget(control_panel)

        # Filter Panel
        filter_panel = QWidget()
        filter_layout = QHBoxLayout(filter_panel)
        filter_label = QLabel("Filter by Date:")
        filter_label.setFont(QFont("Segoe UI", 12))
        self.date_filter_btn = QPushButton("Select Date")
        self.date_filter_btn.setFixedSize(100, 30)
        self.date_filter_btn.clicked.connect(self.show_date_dialog)
        filter_layout.addWidget(filter_label)
        filter_layout.addWidget(self.date_filter_btn)
        packet_tab_layout.addWidget(filter_panel)

        # Splitter for Packets and Details
        splitter = QSplitter(Qt.Vertical)
        self.packet_view = QTableView()
        self.packet_model = PacketTableModel(self.packets, self)
        self.packet_view.setModel(self.packet_model)
        self.packet_view.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.packet_view.setSelectionMode(QTableView.SingleSelection)
        self.packet_view.setSortingEnabled(True)
        self.packet_view.clicked.connect(self.show_packet_details)
        splitter.addWidget(self.packet_view)

        self.details_widget = QWidget()
        details_layout = QVBoxLayout(self.details_widget)
        self.layer_tree = QTreeWidget()
        self.layer_tree.setHeaderHidden(True)
        self.layer_tree.setVisible(False)
        details_layout.addWidget(self.layer_tree)
        self.details_text = QTextEdit()
        self.details_text.setReadOnly(True)
        self.details_text.setFont(QFont("Consolas", 12))
        self.highlighter = PacketHighlighter(self.details_text.document())
        details_layout.addWidget(self.details_text)
        splitter.addWidget(self.details_widget)
        splitter.setSizes([600, 300])
        packet_tab_layout.addWidget(splitter)

        # File Log Tab
        file_log_tab = QWidget()
        file_log_tab_layout = QVBoxLayout(file_log_tab)
        self.file_log_view = QTableView()
        self.file_log_model = FileLogTableModel(self.file_logs, self)
        self.file_log_view.setModel(self.file_log_model)
        self.file_log_view.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.file_log_view.setSelectionMode(QTableView.SingleSelection)
        self.file_log_view.setSortingEnabled(True)
        self.file_log_view.clicked.connect(self.show_file_log_details)
        file_log_tab_layout.addWidget(self.file_log_view)

        self.tab_widget.addTab(packet_tab, QIcon.fromTheme("network-wired"), "Packets")
        self.tab_widget.addTab(file_log_tab, QIcon.fromTheme("folder"), "File Logs")

        # Status Bar
        self.status_bar = QStatusBar()
        self.setStatusBar(self.status_bar)
        self.progress_bar = QProgressBar()
        self.progress_bar.setVisible(False)
        self.status_bar.addPermanentWidget(self.progress_bar)

    def apply_styles(self) -> None:
        palette = QPalette()
        palette.setColor(QPalette.Window, QColor(240, 240, 245))
        palette.setColor(QPalette.WindowText, QColor(30, 30, 30))
        palette.setColor(QPalette.Base, QColor(255, 255, 255))
        palette.setColor(QPalette.AlternateBase, QColor(245, 245, 250))
        palette.setColor(QPalette.Text, QColor(30, 30, 30))
        palette.setColor(QPalette.Button, QColor(220, 220, 220))
        palette.setColor(QPalette.ButtonText, QColor(30, 30, 30))
        palette.setColor(QPalette.Highlight, QColor(0, 122, 255))
        palette.setColor(QPalette.HighlightedText, QColor(255, 255, 255))
        QApplication.setPalette(palette)

        self.setStyleSheet("""
            QMainWindow {
                background-color: #F0F0F5;
            }
            QLabel {
                color: #1E1E1E;
                font-family: 'Segoe UI';
            }
            QPushButton {
                background-color: #E6E6E6;
                border: 1px solid #B0B0B0;
                border-radius: 8px;
                padding: 10px;
                font-family: 'Segoe UI';
                font-size: 14px;
                font-weight: 600;
                color: #1E1E1E;
            }
            QPushButton:hover {
                background-color: #D6D6D6;
                border: 1px solid #A0A0A0;
            }
            QPushButton:pressed {
                background-color: #C6C6C6;
            }
            QTableView {
                background-color: #FFFFFF;
                color: #1E1E1E;
                gridline-color: #E0E0E0;
                selection-background-color: #007AFF;
                selection-color: #FFFFFF;
                font-family: 'Segoe UI';
                font-size: 13px;
                border: 1px solid #D0D0D0;
                border-radius: 5px;
            }
            QHeaderView::section {
                background-color: #F5F5F5;
                color: #1E1E1E;
                padding: 8px;
                border: 1px solid #D0D0D0;
                font-family: 'Segoe UI';
                font-size: 13px;
                font-weight: 600;
            }
            QTextEdit {
                background-color: #FFFFFF;
                color: #1E1E1E;
                border: 1px solid #D0D0D0;
                border-radius: 5px;
                font-family: 'Consolas';
                font-size: 13px;
            }
            QLineEdit {
                background-color: #FFFFFF;
                color: #1E1E1E;
                border: 1px solid #D0D0D0;
                border-radius: 5px;
                padding: 6px;
                font-family: 'Segoe UI';
                font-size: 13px;
            }
            QLineEdit:focus {
                border: 2px solid #007AFF;
            }
            QSplitter::handle {
                background-color: #D0D0D0;
                width: 5px;
            }
            QStatusBar {
                background-color: #F5F5F5;
                color: #1E1E1E;
                font-family: 'Segoe UI';
                font-size: 13px;
            }
            QTabWidget::pane {
                border: 1px solid #D0D0D0;
                border-radius: 5px;
            }
            QTabBar::tab {
                background-color: #E6E6E6;
                color: #1E1E1E;
                padding: 12px;
                border-top-left-radius: 5px;
                border-top-right-radius: 5px;
                font-family: 'Segoe UI';
                font-size: 14px;
            }
            QTabBar::tab:selected {
                background-color: #FFFFFF;
                border-bottom: 2px solid #007AFF;
            }
            QProgressBar {
                background-color: #E6E6E6;
                border: 1px solid #D0D0D0;
                text-align: center;
                color: #1E1E1E;
                border-radius: 5px;
            }
            QProgressBar::chunk {
                background-color: #007AFF;
                border-radius: 5px;
            }
            QToolBar {
                background-color: #F5F5F5;
                border-bottom: 1px solid #D0D0D0;
            }
            QTreeWidget {
                background-color: #FFFFFF;
                color: #1E1E1E;
                border: 1px solid #D0D0D0;
                border-radius: 5px;
                font-family: 'Segoe UI';
                font-size: 13px;
            }
            QTreeWidget::item {
                padding: 5px;
            }
            QTreeWidget::item:hover {
                background-color: #E6F0FA;
            }
        """)

    def animate_button(self, button: QPushButton) -> None:
        animation = QPropertyAnimation(button, b"geometry")
        animation.setDuration(200)
        animation.setEasingCurve(QEasingCurve.OutBounce)
        rect = button.geometry()
        animation.setStartValue(rect)
        animation.setKeyValueAt(0.5, rect.adjusted(5, 5, -5, -5))
        animation.setEndValue(rect)
        animation.start()

    def update_tables(self) -> None:
        self.packet_model.layoutChanged.emit()
        self.file_log_model.layoutChanged.emit()
        packet_scrollbar = self.packet_view.verticalScrollBar()
        file_log_scrollbar = self.file_log_view.verticalScrollBar()
        if packet_scrollbar.maximum() == 0 or packet_scrollbar.value() >= packet_scrollbar.maximum() - 10:
            self.packet_view.scrollToBottom()
        if file_log_scrollbar.maximum() == 0 or file_log_scrollbar.value() >= file_log_scrollbar.maximum() - 10:
            self.file_log_view.scrollToBottom()

    def reload_data(self) -> None:
        if self.current_mode == "stored":
            self.load_stored_logs()
        elif self.current_mode == "current" and self.process and not self.is_paused:
            self.read_sniffer_output()
        self.update_tables()

    def show_stored_logs(self) -> None:
        self.current_mode = "stored"
        self.packets.clear()
        self.details_text.clear()
        self.layer_tree.clear()
        self.stop_capture_btn.setVisible(False)
        self.pause_resume_btn.setVisible(False)
        self.progress_bar.setVisible(True)
        self.progress_bar.setRange(0, 0)
        self.status_bar.showMessage("Loading stored logs...")
        self.load_stored_logs()
        self.progress_bar.setVisible(False)
        self.status_bar.showMessage(f"Stored logs loaded ({len(self.packets)} packets)")
        self.animate_button(self.stored_logs_btn)

    def load_stored_logs(self) -> None:
        log_file = r"C:\Logs\file_operations.log"
        self.packets.clear()
        try:
            if not os.path.exists(log_file):
                QMessageBox.information(self, "Info", "No stored logs found.")
                return

            # Read the file with errors="replace" to handle invalid UTF-8 characters
            with open(log_file, "r", encoding="utf-8", errors="replace") as f:
                content = f.read()

            # Split content into packet blocks using regex, capturing packet number and content
            packet_blocks = re.split(r'===== Packet #(\d+) =====\n', content)
            packet_data = []
            for i in range(1, len(packet_blocks), 2):
                packet_number = packet_blocks[i].strip()
                block_content = packet_blocks[i + 1].strip()
                if block_content:
                    packet_data.append((packet_number, block_content))

            for packet_number, block in packet_data:
                # Split block into lines
                lines = block.split("\n")
                if not lines:
                    continue

                timestamp = ""
                src_ip = "Unknown"
                dst_ip = "Unknown"
                protocol = "Unknown"
                size = "0"
                delta_time = "0.000000"
                details = {}
                current_section = None

                for line in lines:
                    line = line.strip()
                    if not line:
                        continue

                    # Skip CRUD log entries (e.g., [2025-04-28 ...])
                    if re.match(r'\[\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\]', line):
                        continue

                    # Detect section headers (e.g., [Layer 1 - Physical])
                    section_match = re.match(r'\[(.*?)\]', line)
                    if section_match:
                        current_section = section_match.group(1)
                        details[current_section] = []
                        continue

                    # Parse specific fields
                    if current_section == "Timing Information":
                        if line.startswith("Timestamp: "):
                            timestamp = line.split("Timestamp: ", 1)[1].strip()
                        elif line.startswith("Captured Length: "):
                            size = line.split("Captured Length: ", 1)[1].strip().split()[0]
                        elif line.startswith("Delta Time: "):
                            delta_time = line.split("Delta Time: ", 1)[1].strip().split()[0]
                    elif current_section == "Layer 3 - Network":
                        if line.startswith("Source IP: "):
                            src_ip = line.split("Source IP: ", 1)[1].strip()
                        elif line.startswith("Destination IP: "):
                            dst_ip = line.split("Destination IP: ", 1)[1].strip()
                        elif line.startswith("Protocol: "):
                            protocol = line.split("Protocol: ", 1)[1].strip()
                    elif current_section:
                        details[current_section].append(line)

                # Convert lists to strings for each section
                for section in details:
                    details[section] = "\n".join([l for l in details[section] if l.strip()])

                if timestamp:
                    packet = Packet(packet_number, timestamp, src_ip, dst_ip, protocol, size, delta_time, details)
                    self.packets.append(packet)

            self.packet_model.layoutChanged.emit()
            if not self.packets:
                QMessageBox.information(self, "Info", "No valid packets found in the log file.")

        except Exception as e:
            QMessageBox.warning(self, "Error", f"Failed to load logs: {str(e)}")
            with open(r"C:\Logs\debug.log", "a", encoding="utf-8") as f:
                f.write(f"[{time.strftime('%Y-%m-%d %H:%M:%S')}] Error loading logs: {str(e)}\n")

    def apply_date_filter(self, date_str: str) -> None:
        if not date_str:
            return
        try:
            datetime.strptime(date_str, "%Y-%m-%d")
            filtered_packets = [p for p in self.packets if p.timestamp.startswith(date_str)]
            self.packets = filtered_packets
            self.update_tables()
            self.status_bar.showMessage(f"Filtered packets for {date_str} ({len(self.packets)} packets)")
        except ValueError:
            QMessageBox.warning(self, "Warning", "Invalid date format")

    def show_current_traffic(self) -> None:
        self.current_mode = "current"
        self.packets.clear()
        self.details_text.clear()
        self.layer_tree.clear()
        self.stop_capture_btn.setVisible(True)
        self.pause_resume_btn.setVisible(True)
        self.is_paused = False
        self.pause_resume_btn.setText("Pause")
        self.pause_resume_btn.setIcon(QIcon.fromTheme("media-playback-pause"))
        self.progress_bar.setVisible(True)
        self.progress_bar.setRange(0, 0)
        self.status_bar.showMessage("Fetching network interfaces...")

        interfaces = self.get_network_interfaces()
        if not interfaces:
            QMessageBox.critical(self, "Error", "No network interfaces found or failed to run name.exe")
            self.stop_capture()
            return

        dialog = InterfaceDialog(interfaces, self)
        if dialog.exec_() != QDialog.Accepted:
            self.stop_capture()
            return
        interface_index = dialog.selected_index()

        folder_path = QFileDialog.getExistingDirectory(self, "Select Shared Folder", os.path.expanduser("~"))
        if not folder_path:
            self.stop_capture()
            return

        self.start_file_monitoring(folder_path)
        self.start_sniffer_process(interface_index, folder_path)
        self.animate_button(self.current_traffic_btn)

    def get_network_interfaces(self) -> list:
        program = r"C:\Users\kmit\Desktop\name\x64\Debug\name.exe"
        if not os.path.exists(program):
            return []
        process = QProcess(self)
        process.start(program)
        if not process.waitForStarted(3000):
            return []
        process.waitForReadyRead(5000)
        output = process.readAllStandardOutput().data().decode("utf-8", errors="ignore")
        process.kill()
        interfaces = []
        for line in output.split("\n"):
            match = re.match(r"(\d+): (.+)", line.strip())
            if match:
                index, desc = match.groups()
                interfaces.append((int(index), desc.strip()))
        return interfaces

    def start_sniffer_process(self, interface_index: int, folder_path: str) -> None:
        if self.process:
            self.process.kill()
            self.process.waitForFinished(1000)
        self.process = QProcess(self)
        self.process.setProcessChannelMode(QProcess.MergedChannels)
        self.process.readyReadStandardOutput.connect(self.read_sniffer_output)
        self.process.finished.connect(self.process_finished)
        program = r"C:\Users\kmit\Desktop\name\x64\Debug\name.exe"
        if not os.path.exists(program):
            QMessageBox.critical(self, "Error", "name.exe not found!")
            self.stop_capture()
            return
        self.process.start(program)
        if not self.process.waitForStarted(3000):
            QMessageBox.critical(self, "Error", "Failed to start name.exe!")
            self.stop_capture()
            return
        self.process.write(f"{interface_index}\n".encode())
        self.process.write(f"{folder_path}\n".encode())
        self.status_bar.showMessage("Capturing packets...")

    def read_sniffer_output(self) -> None:
        if self.is_paused:
            return
        output = self.process.readAllStandardOutput().data().decode("utf-8", errors="ignore")
        if not output:
            return
        packet_blocks = output.split("===== Packet #")[1:] if "===== Packet #" in output else []
        for block in packet_blocks:
            lines = block.strip().split("\n")
            if not lines:
                continue

            packet_number_match = re.match(r"(\d+)", lines[0])
            if not packet_number_match:
                continue
            packet_number = packet_number_match.group(1)

            timestamp = ""
            src_ip = "Unknown"
            dst_ip = "Unknown"
            protocol = "Unknown"
            size = "0"
            delta_time = "0.000000"
            details = {}
            current_section = None

            for line in lines:
                line = line.strip()
                if not line:
                    continue

                # Detect section headers
                section_match = re.match(r'\[(.*?)\]', line)
                if section_match:
                    current_section = section_match.group(1)
                    details[current_section] = []
                    continue

                # Parse specific fields
                if current_section == "Timing Information":
                    if line.startswith("Timestamp: "):
                        timestamp = line.split("Timestamp: ", 1)[1].strip()
                    elif line.startswith("Captured Length: "):
                        size = line.split("Captured Length: ", 1)[1].strip().split()[0]
                    elif line.startswith("Delta Time: "):
                        delta_time = line.split("Delta Time: ", 1)[1].strip().split()[0]
                elif current_section == "Layer 3 - Network":
                    if line.startswith("Source IP: "):
                        src_ip = line.split("Source IP: ", 1)[1].strip()
                    elif line.startswith("Destination IP: "):
                        dst_ip = line.split("Destination IP: ", 1)[1].strip()
                    elif line.startswith("Protocol: "):
                        protocol = line.split("Protocol: ", 1)[1].strip()
                elif current_section:
                    details[current_section].append(line)

            # Convert lists to strings for each section
            for section in details:
                details[section] = "\n".join([l for l in details[section] if l.strip()])

            if timestamp:
                packet = Packet(packet_number, timestamp, src_ip, dst_ip, protocol, size, delta_time, details)
                self.packets.append(packet)
                self.log_packet_to_file(packet)
                self.log_crud_operation("CAPTURE", f"Packet #{packet_number} captured")

        self.update_tables()

    def log_packet_to_file(self, packet: Packet) -> None:
        log_dir = r"C:\Logs"
        log_file = os.path.join(log_dir, "file_operations.log")
        if not os.path.exists(log_dir):
            os.makedirs(log_dir)
        with open(log_file, "a", encoding="utf-8") as f:
            f.write(f"===== Packet #{packet.number} =====\n")
            for section, content in packet.details.items():
                f.write(f"[{section}]\n{content}\n\n")
            f.write("\n")

    def process_finished(self) -> None:
        if self.process:
            self.process = None
        self.stop_capture_btn.setVisible(False)
        self.pause_resume_btn.setVisible(False)
        self.progress_bar.setVisible(False)
        self.status_bar.showMessage("Packet capture stopped")

    def stop_capture(self) -> None:
        if self.process:
            self.process.write(b"\n")
            self.process.kill()
            self.process.waitForFinished(1000)
            self.process = None
        self.stop_capture_btn.setVisible(False)
        self.pause_resume_btn.setVisible(False)
        self.progress_bar.setVisible(False)
        if self.observer:
            self.observer.stop()
            self.observer.join()
        self.status_bar.showMessage("Packet capture stopped")
        self.animate_button(self.stop_capture_btn)

    def toggle_pause_resume(self) -> None:
        self.is_paused = not self.is_paused
        self.pause_resume_btn.setText("Resume" if self.is_paused else "Pause")
        self.pause_resume_btn.setIcon(QIcon.fromTheme("media-playback-start" if self.is_paused else "media-playback-pause"))
        self.status_bar.showMessage(f"Packet capture {'paused' if self.is_paused else 'resumed'} ({len(self.packets)} packets)")
        self.animate_button(self.pause_resume_btn)

    def show_packet_details(self, index) -> None:
        row = index.row()
        if row < 0 or row >= len(self.packets):
            return
        packet = self.packets[row]
        self.details_text.clear()
        self.layer_tree.clear()

        # Define the order of sections to display
        section_order = [
            "Timing Information",
            "Layer 1 - Physical",
            "Layer 2 - Data Link",
            "Payload",
            "Layer 3 - Network",
            "Layer 4 - Transport",
            "Layer 5 - Session",
            "Layer 6 - Presentation",
            "Layer 7 - Application"
        ]

        if self.layer_details_visible:
            self.layer_tree.setVisible(True)
            self.details_text.setVisible(False)
            for section in section_order:
                if section in packet.details:
                    content = packet.details[section]
                    section_item = QTreeWidgetItem(self.layer_tree, [section])
                    section_item.setExpanded(True)
                    lines = content.split("\n")
                    for line in lines:
                        if line.strip():
                            QTreeWidgetItem(section_item, [line.strip()])
        else:
            self.layer_tree.setVisible(False)
            self.details_text.setVisible(True)
            full_details = ""
            for section in section_order:
                if section in packet.details:
                    content = packet.details[section]
                    full_details += f"[{section}]\n{content}\n\n"
            self.details_text.setText(full_details.strip())

    def toggle_layer_details(self, checked: bool) -> None:
        self.layer_details_visible = checked
        if self.layer_details_visible:
            self.layer_tree.setVisible(True)
            self.details_text.setVisible(False)
        else:
            self.layer_tree.setVisible(False)
            self.details_text.setVisible(True)
        self.show_packet_details(self.packet_view.currentIndex())

    def show_file_log_details(self, index) -> None:
        row = index.row()
        if row < len(self.file_logs):
            log = self.file_logs[row]
            self.details_text.setText(log.details)
            self.layer_tree.setVisible(False)

    def show_date_dialog(self) -> None:
        dialog = DateSelectionDialog(self)
        if dialog.exec_() == QDialog.Accepted:
            self.apply_date_filter(dialog.selected_date())

    def perform_search(self) -> None:
        search_text = self.search_input.text().lower()
        if not search_text:
            self.load_stored_logs()
            self.update_tables()
            return
        filtered_packets = []
        filtered_logs = []
        for packet in self.packets:
            if any(search_text in str(val).lower() for val in [packet.timestamp, packet.src_ip, packet.dst_ip, 
                                                              packet.protocol, packet.size, packet.delta_time, 
                                                              *packet.details.values()]):
                filtered_packets.append(packet)
        for log in self.file_logs:
            if any(search_text in str(val).lower() for val in [log.timestamp, log.operation, log.file_path, log.details]):
                filtered_logs.append(log)
        self.packets = filtered_packets
        self.file_logs = filtered_logs
        self.update_tables()
        self.status_bar.showMessage(f"Search results for '{search_text}' ({len(self.packets)} packets, {len(self.file_logs)} logs)")

    def clear_table(self) -> None:
        self.packets.clear()
        self.file_logs.clear()
        self.update_tables()
        self.details_text.clear()
        self.layer_tree.clear()
        self.status_bar.showMessage("Table cleared")
        self.animate_button(self.clear_table_btn)

    def save_capture(self) -> None:
        if not self.packets:
            QMessageBox.warning(self, "Warning", "No packets to save!")
            return
        file_name, _ = QFileDialog.getSaveFileName(self, "Save Capture", "capture.txt", "Text Files (*.txt)")
        if file_name:
            with open(file_name, "w", encoding="utf-8") as f:
                for packet in self.packets:
                    f.write(f"===== Packet #{packet.number} =====\n")
                    for section, content in packet.details.items():
                        f.write(f"[{section}]\n{content}\n\n")
                    f.write("\n")
            QMessageBox.information(self, "Success", f"Capture saved to {file_name}")
            self.status_bar.showMessage("Capture saved")

    def show_about(self) -> None:
        QMessageBox.information(self, "About", "Advanced Network Monitor\nVersion 1.4\nA professional packet sniffer and log viewer.\nDeveloped with PyQt5.")
        self.status_bar.showMessage("About displayed")

    def delete_stored_traffic(self) -> None:
        log_files = [r"C:\Logs\file_operations.log"]
        try:
            for log_file in log_files:
                if os.path.exists(log_file):
                    with open(log_file, "w", encoding="utf-8") as f:
                        f.write("")
            self.packets.clear()
            self.file_logs.clear()
            self.update_tables()
            QMessageBox.information(self, "Success", "Stored traffic logs deleted.")
            self.status_bar.showMessage("Stored traffic logs deleted")
            self.animate_button(self.delete_logs_btn)
        except Exception as e:
            QMessageBox.warning(self, "Error", f"Failed to delete logs: {str(e)}")

    def start_file_monitoring(self, folder_path: str) -> None:
        if self.observer:
            self.observer.stop()
            self.observer.join()
        event_handler = FileMonitor(folder_path, self)
        self.observer = Observer()
        self.observer.schedule(event_handler, folder_path, recursive=True)
        self.observer.start()

    def log_crud_operation(self, operation: str, details: str) -> None:
        timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
        color = QColor(0, 0, 0)
        log = FileLog(timestamp, operation, "Network Capture", details, color)
        log_dir = r"C:\Logs"
        if not os.path.exists(log_dir):
            os.makedirs(log_dir)
        with open(os.path.join(log_dir, "file_operations.log"), "a", encoding="utf-8") as f:
            f.write(f"[{timestamp}] {operation}: {details}\n")
        self.file_logs.append(log)
        self.file_log_model.layoutChanged.emit()

    def closeEvent(self, event) -> None:
        if self.process:
            self.process.write(b"\n")
            self.process.kill()
            self.process.waitForFinished(1000)
            self.process = None
        if self.observer:
            self.observer.stop()
            self.observer.join()
        self.update_timer.stop()
        self.search_timer.stop()
        self.reload_timer.stop()
        event.accept()

# Application Entry Point
if __name__ == "__main__":
    QApplication.setAttribute(Qt.AA_EnableHighDpiScaling)
    QApplication.setAttribute(Qt.AA_UseHighDpiPixmaps)
    app = QApplication(sys.argv)
    window = NetworkMonitor()
    window.show()
    sys.exit(app.exec_())