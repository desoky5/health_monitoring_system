<?php
header('Content-Type: application/json; charset=utf-8');
header('Cache-Control: no-store');
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: GET, POST, OPTIONS');
header('Access-Control-Allow-Headers: Content-Type');

if ($_SERVER['REQUEST_METHOD'] === 'OPTIONS') {
    http_response_code(204);
    exit;
}

$storageFile = __DIR__ . DIRECTORY_SEPARATOR . 'measurements.json';
$stored = [
    'heartRate' => null,
    'steps' => null,
	'acceleration' => null,
    'updatedAt' => null
];

if (file_exists($storageFile)) {
    $saved = json_decode(file_get_contents($storageFile), true);
    if (is_array($saved)) {
        $stored = array_merge($stored, $saved);
    }
}

if ($_SERVER['REQUEST_METHOD'] === 'GET') {
    echo json_encode($stored);
    exit;
}

if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    http_response_code(405);
    echo json_encode(['error' => 'Use GET or POST']);
    exit;
}

$input = json_decode(file_get_contents('php://input'), true);
if (!is_array($input)) {
    http_response_code(400);
    echo json_encode(['error' => 'Request body must be valid JSON']);
    exit;
}

$hasHeartRate = array_key_exists('heartRate', $input);
$hasSteps = array_key_exists('steps', $input);
$hasAcceleration = array_key_exists('acceleration', $input);
$heartRate = $hasHeartRate ? filter_var($input['heartRate'], FILTER_VALIDATE_FLOAT) : null;
$steps = $hasSteps ? filter_var($input['steps'], FILTER_VALIDATE_FLOAT) : null;
$acceleration = $hasAcceleration ? filter_var($input['acceleration'], FILTER_VALIDATE_FLOAT) : null;

if (!$hasHeartRate && !$hasSteps && !$hasAcceleration) {
    http_response_code(400);
    echo json_encode(['error' => 'Provide heartRate, steps, acceleration, or a combination']);
    exit;
}

if ($hasHeartRate && ($heartRate === false || $heartRate < 0)) {
    http_response_code(400);
    echo json_encode(['error' => 'heartRate must be a non-negative number']);
    exit;
}

if ($hasSteps && ($steps === false || $steps < 0)) {
    http_response_code(400);
    echo json_encode(['error' => 'steps must be a non-negative number']);
    exit;
}

if ($hasAcceleration && ($acceleration === false || $acceleration < 0)) {
    http_response_code(400);
    echo json_encode(['error' => 'acceleration must be a non-negative number']);
    exit;
}

$updated = [
    'heartRate' => $hasHeartRate ? $heartRate : $stored['heartRate'],
    'steps' => $hasSteps ? $steps : $stored['steps'],
	'acceleration' => $hasAcceleration ? $acceleration : $stored['acceleration'],
    'updatedAt' => gmdate('c')
];

if (file_put_contents($storageFile, json_encode($updated), LOCK_EX) === false) {
    http_response_code(500);
    echo json_encode(['error' => 'Could not save measurements']);
    exit;
}

echo json_encode($updated);
