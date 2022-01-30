<?php
require_once __DIR__ . '/../vendor/autoload.php';
require_once __DIR__ . '/../config/config.php';

$app = new Silex\Application();

$app['config'] = $config;

$app->register(new Silex\Provider\TwigServiceProvider(), [
	'twig.path' => __DIR__ . '/../view',
]);

$app->extend('twig', function($twig, $app) {
    return $twig;
});

// index
$app->get('/', function () use ($app) {
    return $app->redirect('https://fosdem.org/schedule/streaming/');
});

// watch
$app->get('/watch/{room}', function ($room) use ($app) {
	if (!isset($app['config']['rooms'][$room])) {
		return $app['twig']->render('nostream.twig', []);
	}

    	$room_slug = $app['config']['rooms'][$room];

    	$chat_name = substr($room_slug, 2);
    	if (substr($room_slug, 0, 1) =='D')
    	{
    	    $chat_name.='-devroom';
    	}

	return $app['twig']->render('watch.twig', [
		'title' => 'Stream ' . $app['config']['rooms'][$room],
		'room' => $room,
		'room_name' => $app['config']['rooms'][$room],
        	'room_slug' => $room_slug,
        	'chat_name' => $chat_name
	]);
});

// list
$app->get('/list', function () use ($app) {
	return $app['twig']->render('list.twig', [
		'title' => 'List',
		'rooms' => $app['config']['rooms'],
	]);
});

$app->run();
