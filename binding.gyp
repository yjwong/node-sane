{
  "targets": [
    {
      "target_name": "sane",
      "sources": [
        "src/sane.cc",
        "src/sane_device.cc",
        "src/sane_handle.cc",
	"src/sane_option_descriptor.cc"
      ],
      "link_settings": {
        "libraries": [
          "-lsane"
        ]
      }
    }
  ],
}
