<?php
header('Content-Type: application/json');
$config = require 'config.php';

try {
    $pdo = new PDO(
        "mysql:host={$config['database']['host']};dbname={$config['database']['dbname']};charset=utf8",
        $config['database']['username'],
        $config['database']['password'],
        [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]
    );
} catch (PDOException $e) {
    http_response_code(500);
    echo json_encode(['status' => 'error', 'message' => 'Database connection failed']);
    exit;
}
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $input = json_decode(file_get_contents('php://input'), true);

    $name = $input['name'] ?? null;
    $voltage = $input['voltage'] ?? null;
    $current = $input['current'] ?? null;
    $power = $input['power'] ?? null;
    $energy = $input['energy'] ?? null;
    $frequency = $input['frequency'] ?? null;
    $power_factor = $input['power_factor'] ?? null;

    // Validate required fields
    if (
        $name === null || $voltage === null || $current === null ||
        $power === null || $energy === null || $frequency === null || $power_factor === null
    ) {
        echo json_encode(['status' => 'error', 'message' => 'Missing required fields']);
        exit;
    }

    $stmt = $pdo->prepare("
        INSERT INTO readings (name, voltage, current, power, energy, frequency, power_factor) 
        VALUES (?, ?, ?, ?, ?, ?, ?)
    ");
    $stmt->execute([$name, $voltage, $current, $power, $energy, $frequency, $power_factor]);

    echo json_encode(['status' => 'success', 'message' => 'Data saved']);
    exit;
}

if ($_SERVER['REQUEST_METHOD'] === 'GET') {
    $limit = isset($_GET['limit']) ? (int)$_GET['limit'] : 10;

    $stmt = $pdo->prepare("SELECT * FROM readings ORDER BY created_at DESC LIMIT ?");
    $stmt->bindValue(1, $limit, PDO::PARAM_INT);
    $stmt->execute();
    $data = $stmt->fetchAll(PDO::FETCH_ASSOC);

    echo json_encode(['status' => 'success', 'data' => $data]);
    exit;
}

echo json_encode(['status' => 'error', 'message' => 'Invalid request method']);
