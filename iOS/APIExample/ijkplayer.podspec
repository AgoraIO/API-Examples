Pod::Spec.new do |s|
  s.name         = "ijkplayer"
  s.version      = "1.1.3"
  s.summary      = "ijkplayer framework."

  s.description  = <<-DESC
bilibili/ijkplayer k0.8.8  IJKMediaFramework 上传到 cococapods
                   DESC

  s.homepage     = "https://github.com/iOSDevLog/ijkplayer"

  s.license      = { :type => "GNU Lesser General Public License v2.1", :text => <<-LICENSE
       GNU LESSER GENERAL PUBLIC LICENSE
       Version 2.1, February 1999
       https://github.com/iOSDevLog/ijkplayer/raw/master/LICENSE
                 LICENSE
               }

  s.author             = { "iosdevlog" => "iosdevlog@iosdevlog.com" }
  s.social_media_url   = "http://weibo.com/iOSDevLog"

  s.platform     = :ios, "7.0"

  s.source       = { :http => "" }
  # s.source       = { :http => "https://github.com/iOSDevLog/ijkplayer/releases/download/#{s.version}/IJKMediaFramework.framework.zip" }

  s.vendored_frameworks = 'IJKMediaFramework.framework'

  s.frameworks  = "AudioToolbox", "AVFoundation", "CoreGraphics", "CoreMedia", "CoreVideo", "MobileCoreServices", "OpenGLES", "QuartzCore", "VideoToolbox", "Foundation", "UIKit", "MediaPlayer"
  s.libraries   = "bz2", "z", "stdc++"

  s.requires_arc = true

end
