{
  "targets": [
    {
      "target_name": "sane",
      "sources": [
        "src/sane.cc",
        "src/sane_device.cc"
      ],
      "link_settings": {
        "libraries": [
          "-lsane"
        ]
      }
    }
  ],
}
