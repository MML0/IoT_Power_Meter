<?php
// migrate.php
header('Content-Type: text/plain');

$config = require 'config.php';

try {
    $pdo = new PDO(
        "mysql:host={$config['database']['host']};dbname={$config['database']['dbname']};charset=utf8",
        $config['database']['username'],
        $config['database']['password'],
        [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]
    );
} catch (PDOException $e) {
    die("Database connection failed: " . $e->getMessage());
}

echo "Connected to database.\n";

// Drop the old table if it exists
$pdo->exec("DROP TABLE IF EXISTS readings");
echo "Dropped old 'readings' table if existed.\n";

// Create fresh table with new columns
$createTableSQL = "
CREATE TABLE readings (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    voltage FLOAT NOT NULL,
    current FLOAT NOT NULL,
    power FLOAT NOT NULL,
    energy FLOAT NOT NULL,
    frequency FLOAT NOT NULL,
    power_factor FLOAT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
";

$pdo->exec($createTableSQL);
echo "Created new 'readings' table with fields: name, voltage, current, power, energy, frequency, power_factor.\n";
