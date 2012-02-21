<?php
	if (array_key_exists("img", $_REQUEST)) {
		$image = $_REQUEST['img'];
		try {
			$img = new Imagick($image);
			if ($img->valid()) {
                if ($img->getImageWidth() > $img->getImageHeight()) {
                    $img->rotateImage(new ImagickPixel('none'), 90);
                }
				$img->thumbnailImage(0, 160);
				if ($img->getImageWidth() > 128) {
					$img->thumbnailImage(128, 0);
				}
				$img->setImageFormat('bmp');
				$img->setCompression(imagick::COMPRESSION_NO);
				$img->setImageDepth(24);
                $img->setImageMatte(false);
                $img->flopImage();
				//$img->setImageAlphaChannel(imagick::ALPHACHANNEL_DEACTIVATE);
                $imgData = (string)$img;
	
				header("Content-Type: image/bmp");
				header("Content-Length: ".strlen($imgData));
				echo $img;
			} else {
				throw new Exception("image not valid");
			}
		} catch (Exception $e) {
			//http_response_code(403);
			echo $e;
		}
	}
?>
