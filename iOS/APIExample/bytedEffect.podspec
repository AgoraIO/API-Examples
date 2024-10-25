Pod::Spec.new do |s|
  s.name         = "bytedEffect"
  s.version      = "4.5.1"
  s.license       = { "type" => "Copyright", "text" => "Copyright 2018 agora.io. All rights reserved.\n"}
  s.homepage     = 'https://github.com'
  s.author        = { "Agora Lab" => "developer@agora.io" }
  s.summary      = 'Demo for effect-sdk'
  s.description   = "iOS library for agora A/V communication, broadcasting and data channel service."
  s.platform      = :ios
  s.frameworks   = 'Accelerate','AssetsLibrary','AVFoundation','CoreGraphics','CoreImage','CoreMedia','CoreVideo','Foundation','QuartzCore','UIKit','CoreMotion','Accelerate','JavaScriptCore'
  s.weak_frameworks = 'Metal','MetalPerformanceShaders', 'Photos', 'CoreML'
  s.source        = { :git => "" }
  # s.source_files  =  "ByteEffectLib/**/*.h"
  # s.public_header_files = "ByteEffectLib/**/*.h"
  # s.header_mappings_dir = "ByteEffectLib/BytedEffectSDK/"
  s.vendored_framework = "ByteEffectLib/*.framework"
  s.vendored_libraries = "ByteEffectLib/*.a"
  s.resource_bundles    = {"ByteEffectLib" => "ByteEffectLib/**/*.bundle"}
  s.platform     = :ios, '9.0'
  s.requires_arc = true
  s.libraries = 'z', 'stdc++'
  s.dependency 'SSZipArchive'
end
