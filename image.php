<?php
	if (array_key_exists("img", $_REQUEST)) {
		$image = $_REQUEST['img'];
		try {
			$img = new Imagick($image);
			if ($img->valid()) {
				$img->thumbnailImage(0, 160);
				if ($img->getImageWidth() > 128) {
					$img->thumbnailImage(128, 0);
				}
				$img->setImageFormat('bmp');
				$img->setCompression(imagick::COMPRESSION_NO);
				$img->setImageDepth(24);
				//$img->setImageAlphaChannel(imagick::ALPHACHANNEL_DEACTIVATE);
	
				header("Content-type: image/bmp");
				echo $img;
			} else {
				throw new Exception("image not valid");
			}
		} catch (Exception $e) {
			http_response_code(403);
			echo $e;
		}
	}
?>
